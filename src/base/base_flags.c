// TODO: new variable value syntex
// TODO: specify shortname and name size
// TODO: Auto add -nocolor and -color flags

typedef enum _Flags_Error_Kind {
    _Flags_Error_Kind_MissingValue,
    _Flags_Error_Kind_UnknownFlag,
    _Flags_Error_Kind_NoFlagPrefix,
    _Flags_Error_Kind_InvalidInt,
    _Flags_Error_Kind_UIntMinus,
    _Flags_Error_Kind_InvalidFloat,
    _Flags_Error_Kind_DuplicateFlag,
    _Flags_Error_Kind_RequireValue,
} _Flags_Error_Kind;

typedef enum _Flags_Flag_Kind
{
    _Flags_Flag_Kind_Str,
    _Flags_Flag_Kind_Bool,
    _Flags_Flag_Kind_Int,
    _Flags_Flag_Kind_UInt,
    _Flags_Flag_Kind_Float,
    _Flags_Flag_Kind_StrArr,
    _Flags_Flag_Kind_IntArr,
    _Flags_Flag_Kind_UIntArr,
    _Flags_Flag_Kind_FloatArr,
} _Flags_Flag_Kind;

typedef struct _Flags_Error _Flags_Error;
struct _Flags_Error
{
    _Flags_Error *next;
    _Flags_Error_Kind kind;
    Str8 flag_name;
    Str8 flag_value;
};

typedef struct Flags_Flag Flags_Flag;
struct Flags_Flag
{
    Flags_Flag *next;
    Str8 name;
    Str8 shortname;
    Str8 description;
    Str8 value_hint;
    union
    {
        Str8 str_value;
        bool bool_value;
        I64 int_value;
        U64 uint_value;
        F64 float_value;
        Str8Array *str_value_arr;
        I64Array *int_value_arr;
        U64Array *uint_value_arr;
        F64Array *float_value_arr;
    } default_value;
    union
    {
        Str8 *str_value;
        bool *bool_value;
        I64 *int_value;
        U64 *uint_value;
        F64 *float_value;
        Str8Array *str_value_arr;
        I64Array *int_value_arr;
        U64Array *uint_value_arr;
        F64Array *float_value_arr;
    } result_value;
    _Flags_Flag_Kind kind;
    bool assigned;
    bool required;
};

typedef struct Flags_Context Flags_Context;
struct Flags_Context
{
    Alloc alloc;
    Flags_Flag *first_flag;
    Flags_Flag *last_flag;
    U64 flags_index;
    _Flags_Error *first_error;
    _Flags_Error *last_error;
    U64 errors_index;
    Str8List error_list;
    bool has_error;
    Str8 program_name;
    bool should_add_color_flags;
    bool has_program_name;
};

internal Flags_Context flags_init(Alloc alloc)
{
    Flags_Context context = ZERO_STRUCT;
    context.alloc = alloc;
    context.has_program_name = true;
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

internal Flags_Flag *flags_int(Flags_Context *context, Str8 name, I64 *result_value, I64 default_value, Str8 description)
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


internal Flags_Flag *flags_uint(Flags_Context *context, Str8 name, U64 *result_value, U64 default_value, Str8 description)
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

internal Flags_Flag *flags_float(Flags_Context *context, Str8 name, F64 *result_value, F64 default_value, Str8 description)
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

internal bool flags_parse(Flags_Context *context, Str8Array *args)
{
    if (context->has_program_name)
    {
        context->program_name = args->strings[0];
    }
    bool has_passthrough_option = false;
    for (U32 i = context->has_program_name ? 1 : 0; i < args->count; i++)
    {
        Str8 option_name = args->strings[i];
        bool is_option = false;
        if (!has_passthrough_option)
        {
            is_option = 1;
            if (str8_match(option_name, str8("--"), 0))
            {
                has_passthrough_option = 1;
                is_option = 0;
            }
            else if (str8_match(str8_prefix(option_name, 2), str8("--"), 0))
            {
                option_name = str8_skip(option_name, 2);
            }
            else if (str8_match(str8_prefix(option_name, 1), str8("-"), 0))
            {
                option_name = str8_skip(option_name, 1);
            }
            else if (Context_Os_CURRENT == Context_Os_Windows &&
                    str8_match(str8_prefix(option_name, 1), str8("/"), 0))
            {
                option_name = str8_skip(option_name, 1);
            }
            else
            {
                _flags_add_error(context, _Flags_Error_Kind_NoFlagPrefix, option_name);
                is_option = 0;
            }
        }

        if (is_option)
        {
            bool has_values = 0;
            U64 value_signifier_position = str8_find_substr(option_name, 0, str8("="), 0);
            Str8 value = str8_skip(option_name, value_signifier_position+1);
            if(value_signifier_position < option_name.size)
            {
                has_values = 1;
            }
            option_name = str8_prefix(option_name, value_signifier_position);
            Flags_Flag *flag = _flags_get_flag(context, option_name);
            if (flag == NULL)
            {
                _flags_add_error(context, _Flags_Error_Kind_UnknownFlag, option_name);
            }
            else if (flag->assigned)
            {
                _flags_add_error(context, _Flags_Error_Kind_DuplicateFlag, option_name);
            }
            else
            {
                U8 splits[] = { ',' };
                Base base = Base_10;
                flag->assigned = true;
                switch (flag->kind)
                {
                    case _Flags_Flag_Kind_Str:
                    {
                        if (has_values)
                        {
                            *flag->result_value.str_value = value;
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_Int:
                    {
                        if (has_values)
                        {
                            if (str8_is_integer(value, base))
                            {
                                *flag->result_value.int_value = str8_to_i64(value, base);
                            }
                            else
                            {
                                _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, option_name, value);
                            }
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_UInt:
                    {
                        if (has_values)
                        {
                            if (str8_is_integer(value, base))
                            {
                                if (str8_is_integer_unsigned(value, base))
                                {
                                    *flag->result_value.int_value = str8_to_u64(value, base);
                                }
                                else
                                {
                                    _flags_add_error_with_value(context, _Flags_Error_Kind_UIntMinus, option_name, value);
                                }
                            }
                            else
                            {
                                _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, option_name, value);
                            }
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_Float:
                    {
                        if (has_values)
                        {
                            if (str8_is_float(value))
                            {
                                *flag->result_value.float_value = str8_to_f64(value);
                            }
                            else
                            {
                                _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidFloat, option_name, value);
                            }
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_Bool:
                    {
                        *flag->result_value.bool_value = true;
                    }
                    break;
                    case _Flags_Flag_Kind_StrArr:
                    {
                        if (has_values)
                        {
                            Str8List str_list = str8_split(context->alloc, value, splits, ArrayCount(splits), 0);
                            Str8Array str_arr = str8_array_from_list(context->alloc, &str_list);
                            *flag->result_value.str_value_arr = str_arr;
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_IntArr:
                    {
                        if (has_values)
                        {
                            Str8List str_list = str8_split(context->alloc, value, splits, ArrayCount(splits), 0);
                            I64Array array = ZERO_STRUCT;
                            array.v = alloc_make(context->alloc, I64, str_list.count);
                            for (Str8Node *node = str_list.first; node != NULL; node = node->next)
                            {
                                if (str8_is_integer(node->string, base))
                                {
                                    array.v[array.count++] = str8_to_i64(node->string, base);
                                }
                                else
                                {
                                    _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, option_name, node->string);
                                }
                            }
                            *flag->result_value.int_value_arr = array;
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_UIntArr:
                    {
                        if (has_values)
                        {
                            Str8List str_list = str8_split(context->alloc, value, splits, ArrayCount(splits), 0);
                            U64Array array = ZERO_STRUCT;
                            array.v = alloc_make(context->alloc, U64, str_list.count);
                            for (Str8Node *node = str_list.first; node != NULL; node = node->next)
                            {
                                if (str8_is_integer(node->string, base))
                                {
                                    if (str8_is_integer_unsigned(node->string, base))
                                    {
                                        array.v[array.count++] = str8_to_u64(node->string, base);
                                    }
                                    else
                                    {
                                        _flags_add_error_with_value(context, _Flags_Error_Kind_UIntMinus, option_name, value);
                                    }
                                }
                                else
                                {
                                    _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, option_name, node->string);
                                }
                            }
                            *flag->result_value.uint_value_arr = array;
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_FloatArr:
                    {
                        if (has_values)
                        {
                            Str8List str_list = str8_split(context->alloc, value, splits, ArrayCount(splits), 0);
                            F64Array array = ZERO_STRUCT;
                            array.v = alloc_make(context->alloc, F64, str_list.count);
                            for (Str8Node *node = str_list.first; node != NULL; node = node->next)
                            {
                                if (str8_is_float(node->string))
                                {
                                    array.v[array.count++] = str8_to_f64(node->string);
                                }
                                else
                                {
                                    _flags_add_error_with_value(context, _Flags_Error_Kind_InvalidInt, option_name, node->string);
                                }
                            }
                            *flag->result_value.float_value_arr = array;
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                }
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
    return !context->has_error;
}

internal void flags_print_error(Flags_Context *context)
{
    Log_Context log_context = log_init();
    for (_Flags_Error *error = context->first_error; error != NULL; error = error->next)
    {
        switch (error->kind)
        {
            case _Flags_Error_Kind_MissingValue:
            {
                log_error(log_context,
                    "flag '%.*s' requires a value. Example: '--%.*s <value>'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UnknownFlag:
            {
                log_error(log_context, "flag '%.*s' is invalid.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_NoFlagPrefix:
            {
                log_error(log_context,
                    "'%.*s' is not recognized as a flag. Flags must start with '-', '--', or '/' (Windows only). Examples: '-%.*s', '--%.*s', '/%.*s'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_DuplicateFlag:
            {
                log_error(log_context, "flag '%.*s' was specified multiple times.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_RequireValue:
            {
                log_error(log_context, "missing required flag '-%.*s'.", str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_InvalidInt:
            {
                log_error(log_context,
                    "flag '%.*s' expects an integer value, but '%.*s' was given. Examples: '--%.*s=42', '--%.*s=-7', '--%.*s 123'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
            }
            break;
            case _Flags_Error_Kind_UIntMinus:
            {
                log_error(log_context,
                    "flag '%.*s' does not accept negative values (got '%.*s'). Use a positive integer instead.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value));
            }
            break;
            case _Flags_Error_Kind_InvalidFloat:
            {
                log_error(log_context,
                    "flag '%.*s' expects a floating-point number, but '%.*s' was given. Examples: '--%.*s .14', '--%.*s -0.5', '--%.*s 2', '--%.*s 2.0'.",
                    str8_varg(error->flag_name), str8_varg(error->flag_value), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name), str8_varg(error->flag_name));
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
        U8 desc_spacing = 5;
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
                    for (U32 i = 0; i < default_array->count; i++)
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
                    for (U32 i = 0; i < default_array->count; i++)
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
                    for (U32 i = 0; i < default_array->count; i++)
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
                    for (U32 i = 0; i < default_array->count; i++)
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
