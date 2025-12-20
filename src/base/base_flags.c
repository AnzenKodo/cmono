internal Flags_Context flags_init(Alloc alloc)
{
    Flags_Context context = ZERO_STRUCT;
    context.alloc = alloc;
    context.has_program_name = true;
    context.log_context = log_init();
    return context;
}

internal Flags_Flag *_flags_get_flag(Flags_Context *context, Str8 name)
{
    Flags_Flag *result = NULL;
    for (Flags_Flag *flag = context->first_flag; flag != NULL; flag = flag->next)
    {
        if (str8_match(name, flag->name, 0))
        {
            result = flag;
        }
        if (str8_match(name, flag->shortname, 0))
        {
            result = flag;
        }
    }
    return result;
}

internal void _flags_add_flag(Flags_Context *context, Flags_Flag *flag)
{
    // Error on finding dublicate flags
    Assert(_flags_get_flag(context, flag->name) == NULL);
    SLLQueuePush(context->first_flag, context->last_flag, flag);
    context->flags_index += 1;
}

internal void flags_add_flag_shortname(Flags_Flag *flag, Str8 shortname)
{
    flag->shortname = shortname;
}
internal void flags_make_flag_required(Flags_Flag *flag)
{
    flag->required = true;
}
internal void flags_add_flag_value_hint(Flags_Flag *flag, Str8 value_hint)
{
    flag->value_hint = value_hint;
}

internal Flags_Flag *flags_string(Flags_Context *context, Str8 name, Str8 *result_value, Str8 default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_Str;
    flag->name = name;
    flag->default_value.str_value = default_value;
    flag->description = description;
    flag->result_value.str_value = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_int(Flags_Context *context, Str8 name, int64_t *result_value, int64_t default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_Int;
    flag->name = name;
    flag->default_value.int_value = default_value;
    flag->description = description;
    flag->result_value.int_value = result_value;
    _flags_add_flag(context, flag);
    return flag;
}


internal Flags_Flag *flags_uint(Flags_Context *context, Str8 name, uint64_t *result_value, uint64_t default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_UInt;
    flag->name = name;
    flag->default_value.uint_value = default_value;
    flag->description = description;
    flag->result_value.uint_value = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_float(Flags_Context *context, Str8 name, double *result_value, double default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_Float;
    flag->name = name;
    flag->default_value.float_value = default_value;
    flag->description = description;
    flag->result_value.float_value = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_bool(Flags_Context *context, Str8 name, bool *result_value, bool default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_Bool;
    flag->name = name;
    flag->default_value.bool_value = default_value;
    flag->description = description;
    flag->result_value.bool_value = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_str_arr(Flags_Context *context, Str8 name, Str8Array *result_value, Str8Array *default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_StrArr;
    flag->name = name;
    flag->default_value.str_value_arr = default_value;
    flag->description = description;
    flag->result_value.str_value_arr = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_int_arr(Flags_Context *context, Str8 name, I64Array *result_value, I64Array *default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_IntArr;
    flag->name = name;
    flag->default_value.int_value_arr = default_value;
    flag->description = description;
    flag->result_value.int_value_arr = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_uint_arr(Flags_Context *context, Str8 name, U64Array *result_value, U64Array *default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_UIntArr;
    flag->name = name;
    flag->default_value.uint_value_arr = default_value;
    flag->description = description;
    flag->result_value.uint_value_arr = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal Flags_Flag *flags_float_arr(Flags_Context *context, Str8 name, F64Array *result_value, F64Array *default_value, Str8 description)
{
    Flags_Flag *flag = alloc_make(context->alloc, Flags_Flag, 1);
    flag->kind = _Flags_Flag_Kind_FloatArr;
    flag->name = name;
    flag->default_value.float_value_arr = default_value;
    flag->description = description;
    flag->result_value.float_value_arr = result_value;
    _flags_add_flag(context, flag);
    return flag;
}

internal void flags_add_color_flags(Flags_Context *context)
{
    context->should_add_color_flags = true;
}

internal void flags_has_program_name(Flags_Context *context, bool has_name)
{
    context->has_program_name = has_name;
}

internal Str8 flags_get_program_name(Flags_Context *context)
{
    return context->program_name;
}

internal void _flags_add_error(Flags_Context *context, _Flags_Error_Kind kind, Str8 name)
{
    _Flags_Error *error = alloc_make(context->alloc, _Flags_Error, 1);
    error->kind = kind;
    error->flag_name = name;
    SLLQueuePush(context->first_error, context->last_error, error);
    context->errors_index += 1;
    context->has_error = true;
}


internal void _flags_add_error_with_value(Flags_Context *context, _Flags_Error_Kind kind, Str8 name, Str8 value)
{
    _Flags_Error *error = alloc_make(context->alloc, _Flags_Error, 1);
    error->kind = kind;
    error->flag_name = name;
    error->flag_value = value;
    SLLQueuePush(context->first_error, context->last_error, error);
    context->errors_index += 1;
    context->has_error = true;
}

internal Str8 _flags_get_options_from_arg(Str8 arg)
{
    Str8 result = ZERO_STRUCT;
    if (str8_match(str8_prefix(arg, 2), str8("--"), 0))
    {
        result = str8_skip(arg, 2);
    }
    else if (str8_match(str8_prefix(arg, 1), str8("-"), 0))
    {
        result = str8_skip(arg, 1);
    }
    else if (Context_Os_CURRENT == Context_Os_Windows &&
            str8_match(str8_prefix(arg, 1), str8("/"), 0))
    {
        result = str8_skip(arg, 1);
    }
    return result;
}

internal bool _flags_is_arg_option(Str8 arg)
{
    return _flags_get_options_from_arg(arg).size > 0 ? true : false;
}

internal uint64_t _flags_get_values_count(Str8Array *args, uint64_t index)
{
    uint64_t count = 0;
    for (uint64_t i = index; i < args->count; i++)
    {
        Str8 arg = args->strings[i];
        if (_flags_is_arg_option(arg)) break;
        count++;
    }
    return count;
}

internal bool flags_parse(Flags_Context *context, Str8Array *args)
{
    bool nocolor = false;
    flags_bool(context, str8("nocolor"), &nocolor, false, str8("Don't print color lines"));
    if (context->has_program_name)
    {
        context->program_name = args->strings[0];
    }
    bool has_passthrough_option = false;
    Flags_Flag *flag = NULL;
    for (uint32_t index = context->has_program_name ? 1 : 0; index < args->count; index++)
    {
        Str8 arg = args->strings[index];
        if (str8_match(arg, str8("--"), 0))
        {
            has_passthrough_option = 1;
            Unused(has_passthrough_option);
            break;
        }
        if (_flags_is_arg_option(arg))
        {
            Str8 option_name = _flags_get_options_from_arg(arg);
            flag = _flags_get_flag(context, option_name);
            if (flag == NULL) {
                _flags_add_error(context, _Flags_Error_Kind_UnknownFlag, option_name);
            }
            else
            {
                if (flag->assigned)
                {
                    _flags_add_error(context, _Flags_Error_Kind_DuplicateFlag, option_name);
                }
                Str8 arg_next = *str8_array_get(args, index+1);
                bool is_arg_next_option = _flags_is_arg_option(arg_next);
                if ((is_arg_next_option || arg_next.size == 0) && flag->kind == _Flags_Flag_Kind_Bool) {
                    *flag->result_value.bool_value = true;
                    flag->assigned = true;
                }
                if (is_arg_next_option && flag->kind != _Flags_Flag_Kind_Bool)
                {
                    _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                }
            }
        }
        else if (flag != NULL)
        {
            if (flag->assigned)
            {
                _flags_add_error(context, _Flags_Error_Kind_SingleValue, flag->name);
            }
            Base base = Base_10;
            flag->assigned = true;
            switch (flag->kind)
            {
                case _Flags_Flag_Kind_Str:
                {
                        *flag->result_value.str_value = arg;
                }
                break;
                case _Flags_Flag_Kind_Int:
                {
                    if (str8_is_integer(arg, base))
                    {
                        *flag->result_value.int_value = str8_to_i64(arg, base);
                    }
                    else
                    {
                        _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, flag->name, arg);
                    }
                }
                break;
                case _Flags_Flag_Kind_UInt:
                {
                    if (str8_is_integer(arg, base))
                    {
                        if (str8_is_integer_unsigned(arg, base))
                        {
                            *flag->result_value.uint_value = str8_to_u64(arg, base);
                        }
                        else
                        {
                            _flags_add_error_with_value(context, _Flags_Error_Kind_UIntMinus, flag->name, arg);
                        }
                    }
                    else
                    {
                        _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, flag->name, arg);
                    }
                }
                break;
                case _Flags_Flag_Kind_Float:
                {
                    if (str8_is_float(arg))
                    {
                        *flag->result_value.float_value = str8_to_f64(arg);
                    }
                    else
                    {
                        _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidFloat, flag->name, arg);
                    }
                }
                break;
                case _Flags_Flag_Kind_Bool:
                {
                    if (str8_is_bool(arg))
                    {
                        *flag->result_value.bool_value = str8_to_bool(arg);
                    }
                    else
                    {
                        _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidBool, flag->name, arg);
                    }
                }
                break;
                case _Flags_Flag_Kind_StrArr:
                {
                    Str8Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.strings = alloc_make(context->alloc, Str8, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->strings[index];
                        array.strings[array.count++] = array_arg;
                        index++;
                    }
                    index--;
                    *flag->result_value.str_value_arr = array;
                }
                break;
                case _Flags_Flag_Kind_IntArr:
                {
                    I64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, int64_t, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->strings[index];
                        if (str8_is_integer(array_arg, base))
                        {
                            array.v[array.count++] = str8_to_i64(array_arg, base);
                        }
                        else
                        {
                            _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, flag->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *flag->result_value.int_value_arr = array;
                }
                break;
                case _Flags_Flag_Kind_UIntArr:
                {
                    U64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, uint64_t, index);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->strings[index];
                        if (str8_is_integer(array_arg, base))
                        {
                            if (str8_is_integer_unsigned(array_arg, base))
                            {
                                array.v[array.count++] = str8_to_u64(array_arg, base);
                            }
                            else
                            {
                                _flags_add_error_with_value(context, _Flags_Error_Kind_UIntMinus, flag->name, array_arg);
                            }
                        }
                        else
                        {
                            _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, flag->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *flag->result_value.uint_value_arr = array;
                }
                break;
                case _Flags_Flag_Kind_FloatArr:
                {
                    F64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, double, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->strings[index];
                        if (str8_is_float(array_arg))
                        {
                            array.v[array.count++] = str8_to_f64(array_arg);
                        }
                        else
                        {
                            _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, flag->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *flag->result_value.float_value_arr = array;
                }
                break;
            }
        }
    }
    // Assign defaults and handle require
    for (Flags_Flag *flag = context->first_flag; flag != NULL; flag = flag->next)
    {
        if (!flag->assigned)
        {
            if (flag->required)
            {
                _flags_add_error(context, _Flags_Error_Kind_RequireValue, flag->name);
            }
            else
            {
                mem_copy(&flag->result_value, &flag->default_value, sizeof(flag->default_value));
            }
        }
    }
    if (nocolor)
    {
        context->log_context.enable_color_log = false;
    }
    return !context->has_error;
}

internal void flags_print_error(Flags_Context *context)
{
    for (_Flags_Error *error = context->first_error; error != NULL; error = error->next)
    {
        switch (error->kind)
        {
            case _Flags_Error_Kind_MissingValue:
            {
                log_error(context->log_context,
                    "flag '%.*s' requires a value. Example: '--%.*s <value>'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UnknownFlag:
            {
                log_error(context->log_context, "flag '%.*s' is invalid.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_NoFlagPrefix:
            {
                log_error(context->log_context,
                    "'%.*s' is not recognized as a flag. Flags must start with '-', '--', or '/' (Windows only). Examples: '-%.*s', '--%.*s', '/%.*s'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_DuplicateFlag:
            {
                log_error(context->log_context, "flag '%.*s' was specified multiple times.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_RequireValue:
            {
                log_error(context->log_context, "missing required flag '-%.*s'.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_InvalidInt:
            {
                log_error(context->log_context,
                    "flag '%.*s' expects an integer value, but '%.*s' was given. Examples: '--%.*s=42', '--%.*s=-7', '--%.*s 123'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UIntMinus:
            {
                log_error(context->log_context,
                    "flag '%.*s' does not accept negative values (got '%.*s'). Use a positive integer instead.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value));
            }
            break;
            case _Flags_Error_Kind_InvalidFloat:
            {
                log_error(context->log_context,
                    "flag '%.*s' expects a floating-point number, but '%.*s' was given. Examples: '--%.*s .14', '--%.*s -0.5', '--%.*s 2', '--%.*s 2.0'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_InvalidBool:
            {
                log_error(context->log_context,
                    "flag '%.*s' expects a boolean value, but '%.*s' was given. Enter 'true', 'false' or no value for true.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value));
            }
            break;
            case _Flags_Error_Kind_SingleValue:
            {
                log_error(context->log_context,
                    "flag '%.*s' only accepts single value.",
                    str8_varg(error->flag_name));
            }
            break;
        }
    }
}

internal void flags_print_help(Flags_Context *context)
{
    for (Flags_Flag *flag = context->first_flag; flag != NULL; flag = flag->next)
    {
        Str8 value_syntex = ZERO_STRUCT;
        Str8 required_syntex = ZERO_STRUCT;
        fmt_printf(" %s%s%s%s-%.*s",
            flag->shortname.size > 0 ? "-" : "",
            flag->shortname.size > 0 ? (char *)flag->shortname.cstr : "",
            flag->shortname.size > 0 ? "," : "",
            flag->shortname.size  ? " " : "",
            str8_varg(flag->name));
        if (flag->value_hint.size > 0)
        {
            fmt_printf(" %.*s", str8_varg(flag->value_hint));
        }
        if (flag->required)
        {
            fmt_print(" (required)");
        }
        uint8_t desc_spacing = 5;
        char *default_syntex = "(default: ";
        fmt_printfln("\n%-*s%.*s", desc_spacing, "", str8_varg(flag->description));
        switch (flag->kind)
        {
            case _Flags_Flag_Kind_Str:
            {
                if (flag->default_value.str_value.size > 0)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    fmt_printfln("\"%.*s\")", str8_varg(flag->default_value.str_value));
                }
            }
            break;
            case _Flags_Flag_Kind_Int:
            {
                if (flag->default_value.int_value != 0)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%lld)", flag->default_value.int_value);
                }
            }
            break;
            case _Flags_Flag_Kind_UInt:
            {
                if (flag->default_value.uint_value != 0)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%llu)", flag->default_value.uint_value);
                }
            }
            break;
            case _Flags_Flag_Kind_Float:
            {
                if (flag->default_value.float_value != 0)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%llf)", flag->default_value.float_value);
                }
            }
            break;
            case _Flags_Flag_Kind_Bool:
            {
            }
            break;
            case _Flags_Flag_Kind_StrArr:
            {
                Str8Array *default_array = flag->default_value.str_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->count; i++)
                    {
                        fmt_printf("\"%s\"", default_array->strings[i]);
                        if (default_array->count-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Flag_Kind_IntArr:
            {
                I64Array *default_array = flag->default_value.int_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->count; i++)
                    {
                        fmt_printf("%lld", default_array->v[i]);
                        if (default_array->count-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Flag_Kind_UIntArr:
            {
                U64Array *default_array = flag->default_value.uint_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->count; i++)
                    {
                        fmt_printf("%llu", default_array->v[i]);
                        if (default_array->count-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Flag_Kind_FloatArr:
            {
                F64Array *default_array = flag->default_value.float_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->count; i++)
                    {
                        fmt_printf("%llf", default_array->v[i]);
                        if (default_array->count-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
        }
    }
}
