// Private functions
// ============================================================================

internal Flags_Option *_flags_get_option(Flags_Context *context, Str8 name)
{
    Flags_Option *result = NULL;
    for (Flags_Option *option = context->first_option; option != NULL; option = option->next)
    {
        if (str8_match(name, option->name, 0))
        {
            result = option;
        }
        if (str8_match(name, option->shortname, 0))
        {
            result = option;
        }
    }
    return result;
}
internal void _flags_add_option(Flags_Context *context, Flags_Option *option)
{
    // Error on finding dublicate flags
    Assert(_flags_get_option(context, option->name) == NULL);
    SLLQueuePush(context->first_option, context->last_option, option);
}

internal Flags_Arg *_flags_get_arg(Flags_Context *context, size_t index)
{
    Flags_Arg *result = NULL;
    for (Flags_Arg *farg = context->first_arg; farg != NULL; farg = farg->next)
    {
        if (farg->index == index)
        {
            result = farg;
        }
    }
    return result;
}
internal void _flags_add_arg(Flags_Context *context, Flags_Arg *farg)
{
    Assert(_flags_get_arg(context, farg->index) == NULL);
    SLLQueuePush(context->first_arg, context->last_arg, farg);
    farg->index = context->index_arg++;
}

internal void _flags_add_option_error(Flags_Context *context, _Flags_Error_Kind kind, Str8 name)
{
    _Flags_Error *error = alloc_make(context->alloc, _Flags_Error, 1);
    error->kind = kind;
    error->flag_name = name;
    SLLQueuePush(context->first_error, context->last_error, error);
    context->has_error = true;
}
internal void _flags_add_option_error_value(Flags_Context *context, _Flags_Error_Kind kind, Str8 name, Str8 value)
{
    _Flags_Error *error = alloc_make(context->alloc, _Flags_Error, 1);
    error->kind = kind;
    error->flag_name = name;
    error->value = value;
    SLLQueuePush(context->first_error, context->last_error, error);
    context->has_error = true;
}
internal void _flags_add_error_arg(Flags_Context *context, _Flags_Error_Kind kind, size_t index, Str8 value)
{
    _Flags_Error *error = alloc_make(context->alloc, _Flags_Error, 1);
    error->kind = kind;
    error->value = value;
    error->arg_index = index;
    SLLQueuePush(context->first_error, context->last_error, error);
    context->has_error = true;
}

internal bool _flags_is_arg_option(Str8 arg)
{
    return _flags_get_options_from_arg(arg).length > 0 ? true : false;
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

internal uint64_t _flags_get_values_count(Str8Array *args, uint64_t index)
{
    uint64_t count = 0;
    for (uint64_t i = index; i < args->length; i++)
    {
        Str8 arg = args->v[i];
        if (_flags_is_arg_option(arg)) break;
        count++;
    }
    return count;
}

// Flags core functions =======================================================

internal Flags_Context flags_init(Alloc alloc)
{
    Flags_Context context = ZERO_STRUCT;
    context.alloc = alloc;
    context.has_program_name = true;
    context.log_context = log_init();
    return context;
}

internal bool flags_parse(Flags_Context *context, Str8Array *args)
{
    bool nocolor = false;
    if (context->should_add_color_flags)
    {
        flags_option_bool(context, str8("nocolor"), &nocolor, false, str8("Don't print color lines"));
    }
    bool has_passthrough_option = false;
    Flags_Option *option = NULL;
    for (uint32_t index = context->has_program_name ? 1 : 0; index < args->length; index++)
    {
        Str8 arg = args->v[index];
        Base base = Base_10;
        if (str8_match(arg, str8("--"), 0))
        {
            has_passthrough_option = 1;
            Unused(has_passthrough_option);
            break;
        }
        if (_flags_is_arg_option(arg))
        {
            Str8 option_name = _flags_get_options_from_arg(arg);
            option = _flags_get_option(context, option_name);
            if (option == NULL) {
                _flags_add_option_error(context, _Flags_Error_Kind_UnknownOption, option_name);
            }
            else
            {
                if (option->assigned)
                {
                    _flags_add_option_error(context, _Flags_Error_Kind_DuplicateOption, option_name);
                }
                Str8 arg_next = *array_get(args, index+1);
                bool is_arg_next_option = _flags_is_arg_option(arg_next);
                if ((is_arg_next_option || arg_next.length == 0) && option->kind == _Flags_Option_Kind_Bool) {
                    *option->result_value.bool_value = true;
                    option->assigned = true;
                }
                if (is_arg_next_option && option->kind != _Flags_Option_Kind_Bool)
                {
                    _flags_add_option_error(context, _Flags_Error_Kind_MissingValue, option_name);
                }
            }
        }
        else if (option != NULL)
        {
            if (option->assigned)
            {
                _flags_add_option_error(context, _Flags_Error_Kind_SingleValue, option->name);
            }
            option->assigned = true;
            switch (option->kind)
            {
                case _Flags_Option_Kind_Str:
                {
                        *option->result_value.str_value = arg;
                }
                break;
                case _Flags_Option_Kind_Int:
                {
                    if (str8_is_integer(arg, base))
                    {
                        *option->result_value.int_value = str8_to_i64(arg, base);
                    }
                    else
                    {
                        _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidIntOption, option->name, arg);
                    }
                }
                break;
                case _Flags_Option_Kind_UInt:
                {
                    if (str8_is_integer(arg, base))
                    {
                        if (str8_is_integer_unsigned(arg, base))
                        {
                            *option->result_value.uint_value = str8_to_u64(arg, base);
                        }
                        else
                        {
                            _flags_add_option_error_value(context, _Flags_Error_Kind_UIntMinusOption, option->name, arg);
                        }
                    }
                    else
                    {
                        _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidIntOption, option->name, arg);
                    }
                }
                break;
                case _Flags_Option_Kind_Float:
                {
                    if (str8_is_float(arg))
                    {
                        *option->result_value.float_value = str8_to_f64(arg);
                    }
                    else
                    {
                        _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidFloatOption, option->name, arg);
                    }
                }
                break;
                case _Flags_Option_Kind_Bool:
                {
                    if (str8_is_bool(arg))
                    {
                        *option->result_value.bool_value = str8_to_bool(arg);
                    }
                    else
                    {
                        _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidBool, option->name, arg);
                    }
                }
                break;
                case _Flags_Option_Kind_StrArr:
                {
                    Str8Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, Str8, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->v[index];
                        array.v[array.length++] = array_arg;
                        index++;
                    }
                    index--;
                    *option->result_value.str_value_arr = array;
                }
                break;
                case _Flags_Option_Kind_IntArr:
                {
                    I64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, int64_t, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->v[index];
                        if (str8_is_integer(array_arg, base))
                        {
                            array.v[array.length++] = str8_to_i64(array_arg, base);
                        }
                        else
                        {
                            _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidIntOption, option->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *option->result_value.int_value_arr = array;
                }
                break;
                case _Flags_Option_Kind_UIntArr:
                {
                    U64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, uint64_t, index);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->v[index];
                        if (str8_is_integer(array_arg, base))
                        {
                            if (str8_is_integer_unsigned(array_arg, base))
                            {
                                array.v[array.length++] = str8_to_u64(array_arg, base);
                            }
                            else
                            {
                                _flags_add_option_error_value(context, _Flags_Error_Kind_UIntMinusOption, option->name, array_arg);
                            }
                        }
                        else
                        {
                            _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidIntOption, option->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *option->result_value.uint_value_arr = array;
                }
                break;
                case _Flags_Option_Kind_FloatArr:
                {
                    F64Array array = ZERO_STRUCT;
                    uint64_t items_count = _flags_get_values_count(args, index);
                    array.v = alloc_make(context->alloc, double, items_count);
                    for (uint64_t i = 0; i < items_count; i++)
                    {
                        Str8 array_arg = args->v[index];
                        if (str8_is_float(array_arg))
                        {
                            array.v[array.length++] = str8_to_f64(array_arg);
                        }
                        else
                        {
                            _flags_add_option_error_value(context, _Flags_Error_Kind_InvalidIntOption, option->name, array_arg);
                        }
                        index++;
                    }
                    index--;
                    *option->result_value.float_value_arr = array;
                }
                break;
            }
        }
        else
        {
            Flags_Arg *farg = _flags_get_arg(context, index - (context->has_program_name ? 1: 0));
            if (farg != NULL)
            {
                farg->assigned = true;
                switch (farg->kind)
                {
                    case _Flags_Arg_Kind_Str:
                    {
                        *farg->value.str_value = arg;
                    }
                    break;
                    case _Flags_Arg_Kind_Int:
                    {
                        if (str8_is_integer(arg, base))
                        {
                            *farg->value.int_value = str8_to_i64(arg, base);
                        }
                        else
                        {
                            _flags_add_error_arg(context, _Flags_Error_Kind_InvalidIntArg, index, arg);
                        }
                    }
                    break;
                    case _Flags_Arg_Kind_UInt:
                    {
                        if (str8_is_integer(arg, base))
                        {
                            if (str8_is_integer_unsigned(arg, base))
                            {
                                *farg->value.uint_value = str8_to_u64(arg, base);
                            }
                            else
                            {
                                _flags_add_error_arg(context, _Flags_Error_Kind_UIntMinusArg, index, arg);
                            }
                        }
                        else
                        {
                            _flags_add_error_arg(context, _Flags_Error_Kind_InvalidIntArg, index, arg);
                        }
                    }
                    break;
                    case _Flags_Arg_Kind_Float:
                    {
                        if (str8_is_float(arg))
                        {
                            *farg->value.float_value = str8_to_f64(arg);
                        }
                        else
                        {
                            _flags_add_error_arg(context, _Flags_Error_Kind_InvalidFloatArg, index, arg);
                        }
                    }
                    break;
                }
            }
            else
            {
                _flags_add_error_arg(context, _Flags_Error_Kind_OutIndexArg, index, arg);
            }
        }
    }
    // Assign defaults and handle require
    for (Flags_Option *option = context->first_option; option != NULL; option = option->next)
    {
        if (!option->assigned)
        {
            if (option->required)
            {
                _flags_add_option_error(context, _Flags_Error_Kind_RequireOption, option->name);
            }
            else
            {
                switch (option->kind)
                {
                    case _Flags_Option_Kind_Str:
                    {
                        *option->result_value.str_value = option->default_value.str_value;
                    }
                    break;
                    case _Flags_Option_Kind_Bool:
                    {
                        *option->result_value.bool_value = option->default_value.bool_value;
                    }
                    break;
                    case _Flags_Option_Kind_Int:
                    {
                        *option->result_value.int_value = option->default_value.int_value;
                    }
                    break;
                    case _Flags_Option_Kind_UInt:
                    {
                        *option->result_value.uint_value = option->default_value.uint_value;
                    }
                    break;
                    case _Flags_Option_Kind_Float:
                    {
                        *option->result_value.float_value = option->default_value.float_value;
                    }
                    break;
                    case _Flags_Option_Kind_StrArr:
                    {
                        option->result_value.str_value_arr = option->default_value.str_value_arr;
                    }
                    break;
                    case _Flags_Option_Kind_IntArr:
                    {
                        option->result_value.int_value_arr = option->default_value.int_value_arr;
                    }
                    break;
                    case _Flags_Option_Kind_UIntArr:
                    {
                        option->result_value.uint_value_arr = option->default_value.uint_value_arr;
                    }
                    break;
                    case _Flags_Option_Kind_FloatArr:
                    {
                        option->result_value.float_value_arr = option->default_value.float_value_arr;
                    }
                    break;
                }
            }
        }
    }
    for (Flags_Arg *farg = context->first_arg; farg != NULL; farg = farg->next)
    {
        if (!farg->assigned)
        {
            if (farg->required)
            {
                _flags_add_error_arg(context, _Flags_Error_Kind_RequireArg, farg->index, str8(""));
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
                    "opiton '%.*s' requires a value. Example: '--%.*s <value>'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UnknownOption:
            {
                log_error(context->log_context, "opiton '%.*s' is invalid.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_NoFlagPrefix:
            {
                log_error(context->log_context,
                    "'%.*s' is not recognized as a opiton. Option must start with '-', '--', or '/' (Windows only). Examples: '-%.*s', '--%.*s', '/%.*s'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_DuplicateOption:
            {
                log_error(context->log_context, "option '%.*s' was specified multiple times.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_RequireOption:
            {
                log_error(context->log_context, "missing required option '--%.*s'.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_InvalidIntOption:
            {
                log_error(context->log_context,
                    "option '%.*s' expects an integer value, but '%.*s' was given. Examples: '--%.*s 42', '--%.*s -7', '--%.*s 123'.",
                    str8_varg(error->flag_name), str8_varg(error->value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UIntMinusOption:
            {
                log_error(context->log_context,
                    "option '%.*s' does not accept negative values (got '%.*s'). Use a positive integer instead.",
                    str8_varg(error->flag_name), str8_varg(error->value));
            }
            break;
            case _Flags_Error_Kind_InvalidFloatOption:
            {
                log_error(context->log_context,
                    "option '%.*s' expects a floating-point number, but '%.*s' was given. Examples: '--%.*s .14', '--%.*s -0.5', '--%.*s 2', '--%.*s 2.0'.",
                    str8_varg(error->flag_name), str8_varg(error->value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_InvalidBool:
            {
                log_error(context->log_context,
                    "option '%.*s' expects a boolean value, but '%.*s' was given. Enter 'true', 'false' or no value for true.",
                    str8_varg(error->flag_name), str8_varg(error->value));
            }
            break;
            case _Flags_Error_Kind_SingleValue:
            {
                log_error(context->log_context,
                    "option '%.*s' only accepts single value.",
                    str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_OutIndexArg:
            {
                log_error(context->log_context,
                    "command only accepts >= (less equal to) %zu arguments without options.",
                    context->index_arg);
            }
            break;
            case _Flags_Error_Kind_RequireArg:
            {
                log_error(context->log_context,
                    "at least %zu argument without option is required.",
                    error->arg_index + 1);
            }
            break;
            case _Flags_Error_Kind_InvalidIntArg:
            {
                log_error(context->log_context,
                    "%zu argument without option should be integer. Examples: '42', '-7', '123'.",
                    error->arg_index + 1);
            }
            break;
            case _Flags_Error_Kind_UIntMinusArg:
            {
                log_error(context->log_context,
                    "%zu argument without option does not accept negative values (got '%.*s'). Use a positive integer instead.",
                    error->arg_index + 1, str8_varg(error->value));
            }
            break;
            case _Flags_Error_Kind_InvalidFloatArg:
            {
                log_error(context->log_context,
                    "%zu argument without option expects a floating-point number, but '%.*s' was given. Examples: '.14', '-0.5', '2', '2.0'.",
                    error->arg_index + 1, str8_varg(error->value));
            }
            break;
        }
    }
}
internal void flags_print_help(Flags_Context *context)
{
    for (Flags_Option *option = context->first_option; option != NULL; option = option->next)
    {
        Str8 value_syntex = ZERO_STRUCT;
        Str8 required_syntex = ZERO_STRUCT;
        term_style_start(TERM_BOLD);
        fmt_printf("    %s%s%s%s--%.*s",
            option->shortname.length > 0 ? "-" : "",
            option->shortname.length > 0 ? (char *)option->shortname.cstr : "",
            option->shortname.length > 0 ? "," : "",
            option->shortname.length  ? " " : "",
            str8_varg(option->name));
        if (option->value_hint.length > 0)
        {
            fmt_printf(" %.*s", str8_varg(option->value_hint));
        }
        term_style_end();
        if (option->required)
        {
            fmt_printf(" (required)");
        }
        uint8_t desc_spacing = 8;
        char *default_syntex = "(default: ";
        fmt_printfln("\n%-*s%.*s", desc_spacing, "", str8_varg(option->description));
        switch (option->kind)
        {
            case _Flags_Option_Kind_Str:
            {
                if (option->default_value.str_value.length > 0)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    fmt_printfln("\"%.*s\")", str8_varg(option->default_value.str_value));
                }
            }
            break;
            case _Flags_Option_Kind_Int:
            {
                if (option->default_value.int_value != 0)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%lld)", option->default_value.int_value);
                }
            }
            break;
            case _Flags_Option_Kind_UInt:
            {
                if (option->default_value.uint_value != 0)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%llu)", option->default_value.uint_value);
                }
            }
            break;
            case _Flags_Option_Kind_Float:
            {
                if (option->default_value.float_value != 0)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    fmt_printfln("%llf)", option->default_value.float_value);
                }
            }
            break;
            case _Flags_Option_Kind_Bool:
            {
            }
            break;
            case _Flags_Option_Kind_StrArr:
            {
                Str8Array *default_array = option->default_value.str_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->length; i++)
                    {
                        fmt_printf("\"%s\"", default_array->v[i]);
                        if (default_array->length-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Option_Kind_IntArr:
            {
                I64Array *default_array = option->default_value.int_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->length; i++)
                    {
                        fmt_printf("%lld", default_array->v[i]);
                        if (default_array->length-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Option_Kind_UIntArr:
            {
                U64Array *default_array = option->default_value.uint_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->length; i++)
                    {
                        fmt_printf("%llu", default_array->v[i]);
                        if (default_array->length-1 != i)
                        {
                            fmt_print(" ");
                        }
                    }
                    fmt_printfln(")");
                }
            }
            break;
            case _Flags_Option_Kind_FloatArr:
            {
                F64Array *default_array = option->default_value.float_value_arr;
                if (default_array != NULL)
                {
                    fmt_printf("%*.s%s", desc_spacing, "", default_syntex);
                    for (uint32_t i = 0; i < default_array->length; i++)
                    {
                        fmt_printf("%llf", default_array->v[i]);
                        if (default_array->length-1 != i)
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

// Flags config functions
// ============================================================================

internal void flags_add_color_flags(Flags_Context *context)
{
    context->should_add_color_flags = true;
}
internal void flags_has_program_name(Flags_Context *context, bool has_name)
{
    context->has_program_name = has_name;
}

internal void flags_add_option_shortname(Flags_Option *option, Str8 shortname)
{
    option->shortname = shortname;
}
internal void flags_add_option_value_hint(Flags_Option *option, Str8 value_hint)
{
    option->value_hint = value_hint;
}
internal void flags_make_option_required(Flags_Option *option)
{
    option->required = true;
}
internal void flags_make_arg_required(Flags_Arg *farg)
{
    farg->required = true;
}

// Add option ===================================================================

internal Flags_Option *flags_option_string(Flags_Context *context, Str8 name, Str8 *result_value, Str8 default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_Str;
    option->name = name;
    option->default_value.str_value = default_value;
    option->description = description;
    option->result_value.str_value = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_int(Flags_Context *context, Str8 name, int64_t *result_value, int64_t default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_Int;
    option->name = name;
    option->default_value.int_value = default_value;
    option->description = description;
    option->result_value.int_value = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_uint(Flags_Context *context, Str8 name, uint64_t *result_value, uint64_t default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_UInt;
    option->name = name;
    option->default_value.uint_value = default_value;
    option->description = description;
    option->result_value.uint_value = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_float(Flags_Context *context, Str8 name, double *result_value, double default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_Float;
    option->name = name;
    option->default_value.float_value = default_value;
    option->description = description;
    option->result_value.float_value = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_bool(Flags_Context *context, Str8 name, bool *result_value, bool default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_Bool;
    option->name = name;
    option->default_value.bool_value = default_value;
    option->description = description;
    option->result_value.bool_value = result_value;
    _flags_add_option(context, option);
    return option;
}

internal Flags_Option *flags_option_str_arr(Flags_Context *context, Str8 name, Str8Array *result_value, Str8Array *default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_StrArr;
    option->name = name;
    option->default_value.str_value_arr = default_value;
    option->description = description;
    option->result_value.str_value_arr = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_int_arr(Flags_Context *context, Str8 name, I64Array *result_value, I64Array *default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_IntArr;
    option->name = name;
    option->default_value.int_value_arr = default_value;
    option->description = description;
    option->result_value.int_value_arr = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_uint_arr(Flags_Context *context, Str8 name, U64Array *result_value, U64Array *default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_UIntArr;
    option->name = name;
    option->default_value.uint_value_arr = default_value;
    option->description = description;
    option->result_value.uint_value_arr = result_value;
    _flags_add_option(context, option);
    return option;
}
internal Flags_Option *flags_option_float_arr(Flags_Context *context, Str8 name, F64Array *result_value, F64Array *default_value, Str8 description)
{
    Flags_Option *option = alloc_make(context->alloc, Flags_Option, 1);
    option->kind = _Flags_Option_Kind_FloatArr;
    option->name = name;
    option->default_value.float_value_arr = default_value;
    option->description = description;
    option->result_value.float_value_arr = result_value;
    _flags_add_option(context, option);
    return option;
}

// Add value flag
// ============================================================================

internal Flags_Arg *flags_arg_str(Flags_Context *context, Str8 *value)
{
    Flags_Arg *farg = alloc_make(context->alloc, Flags_Arg, 1);
    farg->kind = _Flags_Arg_Kind_Str;
    farg->value.str_value = value;
    _flags_add_arg(context, farg);
    return farg;
}
internal Flags_Arg *flags_arg_int(Flags_Context *context, int64_t *value)
{
    Flags_Arg *farg = alloc_make(context->alloc, Flags_Arg, 1);
    farg->kind = _Flags_Arg_Kind_Int;
    farg->value.int_value = value;
    _flags_add_arg(context, farg);
    return farg;
}
internal Flags_Arg *flags_arg_uint(Flags_Context *context, uint64_t *value)
{
    Flags_Arg *farg = alloc_make(context->alloc, Flags_Arg, 1);
    farg->kind = _Flags_Arg_Kind_UInt;
    farg->value.uint_value = value;
    _flags_add_arg(context, farg);
    return farg;
}
internal Flags_Arg *flags_arg_float(Flags_Context *context, double *value)
{
    Flags_Arg *farg = alloc_make(context->alloc, Flags_Arg, 1);
    farg->kind = _Flags_Arg_Kind_Float;
    farg->value.float_value = value;
    _flags_add_arg(context, farg);
    return farg;
}

