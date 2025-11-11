typedef enum _Flags_Error_Kind {
    _Flags_Error_Kind_None,
    _Flags_Error_Kind_MissingValue,
    _Flags_Error_Kind_UnknownFlag,
    _Flags_Error_Kind_InvalidFlagPrefix,
    // _Flags_Error_Kind_Unassigned_flag,
    // _Flags_Error_Kind_Invalid_int,
    // _Flags_Error_Kind_Invalid_double,
    // _Flags_Error_Kind_Too_many_non_flag_args,
    // _Flags_Error_Kind_Multiple_assignment,
    // _Flags_Error_Kind_Prefix_no,
} _Flags_Error_Kind;

typedef enum _Flags_Flag_Kind
{
    _Flags_Flag_Kind_None,
    _Flags_Flag_Kind_Str,
    _Flags_Flag_Kind_Bool,
    _Flags_Flag_Kind_Int,
    _Flags_Flag_Kind_Float,
    _Flags_Flag_Kind_StrArr,
    _Flags_Flag_Kind_IntArr,
    _Flags_Flag_Kind_FloatArr,
} _Flags_Flag_Kind;

typedef struct _Flags_Error _Flags_Error;
struct _Flags_Error
{
    _Flags_Error_Kind kind;
    Str8 flag_name;
};

typedef struct _Flags_Flag _Flags_Flag;
struct _Flags_Flag
{
    Str8 name;
    Str8 description;
    union
    {
        Str8 str_value;
        bool bool_value;
        I64 int_value;
        F64 float_value;
    } default_value;
    union
    {
        Str8 *str_value;
        bool *bool_value;
        I64 *int_value;
        F64 *float_value;
        // kgflags_string_array_t *string_array;
        // kgflags_int_array_t *int_array;
        // kgflags_double_array_t *double_array;
    } result_value;
    _Flags_Flag_Kind kind;
    bool assigned;
    bool required;
};

#ifndef FLAGS_MAX_FLAGS
#   define FLAGS_MAX_FLAGS 256
#endif

#ifndef FLAGS_MAX_ERRORS
#   define FLAGS_MAX_ERRORS FLAGS_MAX_FLAGS
#endif

typedef struct Flags_Context Flags_Context;
struct Flags_Context
{
    Str8 program_name;
    _Flags_Flag flags[FLAGS_MAX_FLAGS];
    _Flags_Error errors[FLAGS_MAX_ERRORS];
    U64 errors_index;
    U64 flags_index;
    bool has_error;
};

internal Flags_Context flags_init(void)
{
    Flags_Context context = ZERO_STRUCT;
    return context;
}

internal _Flags_Flag *_flags_get_flag(Flags_Context *context, Str8 name)
{
    _Flags_Flag *result = NULL;
    for (int i = 0; i < context->flags_index; i++)
    {
        _Flags_Flag *flag = &context->flags[i];
        if (str8_match(name, flag->name, 0))
        {
            result = flag;
        }
    }
    return result;
}

internal void _flags_add_flag(Flags_Context *context, _Flags_Flag flag)
{
    // Error on hitting max number of flags
    Assert(context->flags_index <= FLAGS_MAX_FLAGS);
    // Error on finding dublicate flags
    Assert(_flags_get_flag(context, flag.name) == NULL);
    context->flags[context->flags_index++] = flag;
}

internal void flags_string(Flags_Context *context, Str8 name, Str8 *result_value, Str8 default_value, Str8 description)
{
    _Flags_Flag flag = ZERO_STRUCT;
    flag.kind = _Flags_Flag_Kind_Str;
    flag.name = name;
    flag.default_value.str_value = default_value;
    flag.description = description;
    flag.result_value.str_value = result_value;
    flag.assigned = false;
    _flags_add_flag(context, flag);
}

internal void flags_add_flag_name(Flags_Context *context, Str8 name)
{
}

internal void flags_make_flag_required(Flags_Context *context, Str8 name)
{
}

internal void _flags_add_error(Flags_Context *context, _Flags_Error_Kind kind, Str8 name)
{
    _Flags_Error error = ZERO_STRUCT;
    error.kind = kind;
    error.flag_name = name;
    context->errors[context->errors_index++] = error;
    context->has_error = true;
}

internal bool flags_parse(Flags_Context *context, Str8Array *args)
{
    context->program_name = args->strings[0];
    bool has_passthrough_option = false;
    for (U32 i = 1; i < args->length; i++)
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
                _flags_add_error(context, _Flags_Error_Kind_InvalidFlagPrefix, option_name);
                is_option = 0;
            }
        }

        if (is_option)
        {
            bool has_values = 0;
            U64 value_signifier_position = str8_find_needle(option_name, 0, str8("="), 0);
            Str8 value_portion_after_string = str8_skip(option_name, value_signifier_position+1);
            if(value_signifier_position < option_name.size)
            {
                has_values = 1;
            }
            option_name = str8_prefix(option_name, value_signifier_position);
            _Flags_Flag *flag = _flags_get_flag(context, option_name);
            if (flag == NULL)
            {
                _flags_add_error(context, _Flags_Error_Kind_UnknownFlag, option_name);
            }
            else
            {
                switch (flag->kind)
                {
                    case _Flags_Flag_Kind_None:
                    {
                    }
                    break;
                    case _Flags_Flag_Kind_Str:
                    {
                        if (has_values)
                        {
                            *flag->result_value.str_value = str8_postfix(args->strings[i], value_signifier_position);
                        }
                        else
                        {
                            _flags_add_error(context, _Flags_Error_Kind_MissingValue, option_name);
                        }
                    }
                    break;
                    case _Flags_Flag_Kind_Int:
                    {
                    }
                    break;
                    case _Flags_Flag_Kind_Float:
                    {
                    }
                    break;
                    case _Flags_Flag_Kind_Bool:
                    {
                        // flag->result_value.bool_value = true;
                    }
                    break;
                    case _Flags_Flag_Kind_StrArr:
                    {
                    }
                    break;
                    case _Flags_Flag_Kind_IntArr:
                    {
                    }
                    break;
                    case _Flags_Flag_Kind_FloatArr:
                    {
                    }
                    break;
                }
            }
        }
        // Str8 key = args->strings[i];
        // Str8 *value = 0;
        // if (i+1 < args->length)
        // {
        //     value = &args->strings[i+1];
        // }
        // _Flags_Flag *flag = _flags_get_flag(context, key);
        // if (flag == NULL)
        // {
        //     _flags_add_error(context, _Flags_Error_Kind_UnknownFlag, key);
        // }
        // else
        // {
        //     switch (flag->kind)
        //     {
        //         case _Flags_Flag_Kind_None:
        //         {
        //         }
        //         break;
        //         case _Flags_Flag_Kind_Str:
        //         {
        //             if (value == NULL)
        //             {
        //                 _flags_add_error(context, _Flags_Error_Kind_MissingValue, key);
        //             }
        //             *flag->result_value.str_value = *value;
        //             i++;
        //         }
        //         break;
        //         case _Flags_Flag_Kind_Int:
        //         {
        //             if (value == NULL)
        //             {
        //                 _flags_add_error(context, _Flags_Error_Kind_MissingValue, key);
        //             }
        //             *flag->result_value.int_value = *value;
        //             i++;
        //         }
        //         break;
        //         case _Flags_Flag_Kind_Float:
        //         {
        //             if (value == NULL)
        //             {
        //                 _flags_add_error(context, _Flags_Error_Kind_MissingValue, key);
        //             }
        //             *flag->result_value.float_value = *value;
        //             i++;
        //         }
        //         break;
        //         case _Flags_Flag_Kind_Bool:
        //         {
        //             // flag->result_value.bool_value = true;
        //         }
        //         break;
        //         case _Flags_Flag_Kind_StrArr:
        //         {
        //         }
        //         break;
        //         case _Flags_Flag_Kind_IntArr:
        //         {
        //         }
        //         break;
        //         case _Flags_Flag_Kind_FloatArr:
        //         {
        //         }
        //         break;
        //     }
        // }
    }
    return !context->has_error;
}

internal void flags_print_error(Flags_Context *context)
{
    for (int i = 0; i < context->errors_index; i++)
    {
        _Flags_Error *error = &context->errors[i];
        switch (error->kind)
        {
            case _Flags_Error_Kind_None:
            {
                fmt_println("_Flags_Error_Kind_None");
            }
            break;
            case _Flags_Error_Kind_MissingValue:
            {
                fmt_println("_Flags_Error_Kind_MissingValue");
            }
            break;
            case _Flags_Error_Kind_UnknownFlag:
            {
                fmt_println("_Flags_Error_Kind_UnknownFlag");
            }
            break;
            case _Flags_Error_Kind_InvalidFlagPrefix:
            {
                fmt_println("_Flags_Error_Kind_InvalidFlagPrefix");
            }
            break;
        }
    }
}

/*
> program --help
USAGE:
  program [COMMAND] [...OPTIONS]
COMMAND:
  run   Run the command
  kill  Delete the program process
OPTIONS:
  -h --help        Print help
  -v --version     Print version
> program run --help
USAGE:
  program run [COMMAND] [...OPTIONS] <filename>
COMMAND:
  single    Run single file
  multi     Run multiple file
OPTIONS:
  -h --help     Print help
  -v --version  Print version
> program run single --help
USAGE:
  program run single [...OPTIONS] <filename>
OPTIONS:
  -h --help      Print help
  -v --version   Print version
*/
