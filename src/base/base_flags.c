typedef enum _Flags_Flag_Kind {
    _Flags_Flag_Kind_None,
    _Flags_Flag_Kind_Str,
    _Flags_Flag_Kind_Bool,
    _Flags_Flag_Kind_Int,
    _Flags_Flag_Kind_Float,
    _Flags_Flag_Kind_StrArr,
    _Flags_Flag_Kind_IntArr,
    _Flags_Flag_Kind_FloatArr,
} _Flags_Flag_Kind;

typedef struct Flags_Flag Flags_Flag;
struct Flags_Flag
{
    Str8 name;
    Str8 description;
    union {
        Str8 str_value;
        bool bool_value;
        I64 int_value;
        F64 float_value;
    } default_value;
    union {
        Str8 *str_value;
        bool *bool_value;
        I64 *int_value;
        F64 *float_value;
        // kgflags_string_array_t *string_array;
        // kgflags_int_array_t *int_array;
        // kgflags_double_array_t *double_array;
    } result_value;
    bool assigned;
    bool error;
    bool required;
    _Flags_Flag_Kind kind;
};

#ifndef FLAGS_MAX_FLAGS
#   define FLAGS_MAX_FLAGS 256
#endif

typedef struct Flags_State Flags_State;
struct Flags_State
{
    Str8 program_name;
    U64 table_size;
    Flags_Flag flags[FLAGS_MAX_FLAGS];
};

internal Flags_State flags_init(void)
{
    Flags_State state = ZERO_STRUCT;
    state.table_size = FLAGS_MAX_FLAGS;
    return state;
}

internal void _flags_add_flag(Flags_State *state, Flags_Flag flag)
{
    U64 hash = str8_hash_u64(flag.name);
    U64 bucket = hash % state->table_size;
    state->flags[bucket] = flag;
}

internal void flags_string(Flags_State *state, Str8 name, Str8 *result_value, Str8 default_value, Str8 description)
{
    Flags_Flag flag = ZERO_STRUCT;
    flag.kind = _Flags_Flag_Kind_Str;
    flag.name = name;
    flag.default_value.str_value = default_value;
    flag.description = description;
    flag.result_value.str_value = result_value;
    flag.assigned = false;
    _flags_add_flag(state, flag);
}

internal void flags_add_flag_name(Flags_State *state, Str8 name)
{
}

internal void flags_make_flag_required(Flags_State *state, Str8 name)
{
}

internal void flags_parse(Flags_State *state, Str8Array *args)
{
    state->program_name = args->strings[0];
    for (U32 i = 1; i < args->length; i++)
    {
        Str8 name = args->strings[i];
        U64 hash = str8_hash_u64(name);
        U64 bucket = hash % state->table_size;
        Flags_Flag flag = state->flags[bucket];
        switch (flag.kind) {
            case _Flags_Flag_Kind_None:
            {
            } break;
            case _Flags_Flag_Kind_Str:
            {
            } break;
            case _Flags_Flag_Kind_Bool:
            {
            } break;
            case _Flags_Flag_Kind_Int:
            {
            } break;
            case _Flags_Flag_Kind_Float:
            {
            } break;
            case _Flags_Flag_Kind_StrArr:
            {
            } break;
            case _Flags_Flag_Kind_IntArr:
            {
            } break;
            case _Flags_Flag_Kind_FloatArr:
            {
            } break;
        }
        fmt_printf("%s", flag.name.cstr);
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
