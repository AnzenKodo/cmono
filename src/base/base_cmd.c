// Command Line Parsing Functions
//=============================================================================

internal CmdLineOpt *
cmd_opt_from_slot(CmdLineOpt **slot, Str8 string)
{
    CmdLineOpt *result = 0;
    for(CmdLineOpt *var = *slot; var; var = var->hash_next)
    {
        if(str8_match(string, var->string, 0))
        {
            result = var;
            break;
        }
    }
    return result;
}

internal CmdLineOpt **
cmd_slot_from_string(CmdLine *cmd, Str8 string)
{
    CmdLineOpt **slot = 0;
    if(cmd->option_table_size != 0)
    {
        U64 hash = str8_hash_u64(string);
        U64 bucket = hash % cmd->option_table_size;
        slot = &cmd->option_table[bucket];
    }
    return slot;
}

internal CmdLine cmd_parsed(Alloc alloc, Str8List args_list)
{
    CmdLine parsed = ZERO_STRUCT;
    parsed.exe_name = args_list.first->string;
    parsed.option_table_size = 64;
    parsed.option_table = alloc_make(alloc, CmdLineOpt *, parsed.option_table_size);
    bool after_passthrough_option = 0;
    bool first_passthrough = 1;
    for(Str8Node *node = args_list.first->next, *next = 0; node != 0; node = next)
    {
        next = node->next;
        bool is_option = false;
        Str8 option_arg = node->string;
        if(!after_passthrough_option)
        {
            is_option = 1;
            if(str8_match(node->string, str8("--"), 0)) {
                after_passthrough_option = 1;
                is_option = 0;
            } else if(str8_match(str8_prefix(node->string, 2), str8("--"), 0)) {
                option_arg = str8_skip(option_arg, 2);
            } else if(str8_match(str8_prefix(node->string, 1), str8("-"), 0)) {
                option_arg = str8_skip(option_arg, 1);
            } else if(
                context_of_os() == Context_Os_Windows &&
                str8_match(str8_prefix(node->string, 1), str8("/"), 0)
            ) {
                option_arg = str8_skip(option_arg, 1);
            } else {
                is_option = 0;
            }
        }
        if (is_option) {
            bool has_values = 0;
            U64 value_signifier_position_colan = str8_find_substr(
                option_arg, 0, str8(":"), 0
            );
            U64 value_signifier_position_equal = str8_find_substr(
                option_arg, 0, str8("="), 0
            );
            U64 value_signifier_position = Min(
                value_signifier_position_colan, value_signifier_position_equal
            );
            Str8 value_portion_this_string = str8_skip(
                option_arg, value_signifier_position+1
            );
            if (value_signifier_position < option_arg.size)
            {
                has_values = 1;
            }
            Str8 option_name = str8_prefix(option_arg, value_signifier_position);
            Str8List values = ZERO_STRUCT;
            if (has_values)
            {
                for (Str8Node *n = node; n; n = n->next)
                {
                    next = n->next;
                    Str8 string = n->string;
                    if (n == node)
                    {
                        string = value_portion_this_string;
                    }
                    U8 splits[] = { ',' };
                    Str8List values_in_this_string = str8_split(
                        alloc, string, splits, ArrayCount(splits), 0
                    );
                    for (
                        Str8Node *sub_val = values_in_this_string.first;
                        sub_val;
                        sub_val = sub_val->next
                    ) {
                        str8_list_push(alloc, &values, sub_val->string);
                    }
                    if (
                        !str8_match(str8_postfix(n->string, 1), str8(","), 0) &&
                        (n != node || value_portion_this_string.size != 0)
                    ) {
                        break;
                    }
                }
            }
            // Store cmd
            CmdLineOpt *var = 0;
            CmdLineOpt **slot = cmd_slot_from_string(&parsed, option_name);
            CmdLineOpt *existing_var = cmd_opt_from_slot(slot, option_name);
            if(existing_var != 0) {
                var = existing_var;
            } else {
                var = alloc_make(alloc, CmdLineOpt, 1);
                var->hash_next = *slot;
                var->hash = str8_hash_u64(option_name);
                var->string = str8_copy(alloc, option_name);
                var->value_strings = values;
                StrJoin join = ZERO_STRUCT;
                join.pre = str8("");
                join.sep = str8(",");
                join.post = str8("");
                var->value_string = str8_list_join(alloc, &var->value_strings, &join);
                *slot = var;
                SLLPush(parsed.options.first, parsed.options.last, var);
                parsed.options.count += 1;
            }
        } else if(!str8_match(node->string, str8("--"), 0) || !first_passthrough) {
            // default path - treat as a passthrough input
            str8_list_push(alloc, &parsed.inputs, node->string);
            first_passthrough = 0;
        }
    }
    parsed.args_list = args_list;
    return parsed;
}

internal CmdLineOpt *
cmd_opt_from_string(CmdLine *cmd, Str8 name)
{
    return cmd_opt_from_slot(cmd_slot_from_string(cmd, name), name);
}

internal Str8List
cmd_strings(CmdLine *cmd, Str8 name)
{
    Str8List result = {0};
    CmdLineOpt *var = cmd_opt_from_string(cmd, name);
    if(var != 0)
    {
        result = var->value_strings;
    }
    return result;
}

internal Str8
cmd_string(CmdLine *cmd, Str8 name)
{
    Str8 result = {0};
    CmdLineOpt *var = cmd_opt_from_string(cmd, name);
    if(var != 0)
    {
        result = var->value_string;
    }
    return result;
}

internal bool
cmd_has_flag(CmdLine *cmd, Str8 name)
{
    CmdLineOpt *var = cmd_opt_from_string(cmd, name);
    return (var != 0);
}

internal bool
cmd_has_argument(CmdLine *cmd, Str8 name)
{
    CmdLineOpt *var = cmd_opt_from_string(cmd, name);
    return (var != 0 && var->value_strings.count > 0);
}

