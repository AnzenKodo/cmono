#ifndef BASE_FLAGS_H
#define BASE_FLAGS_H

// Types
// ============================================================================

typedef enum _Flags_Error_Kind
{
    _Flags_Error_Kind_MissingValue,
    _Flags_Error_Kind_UnknownFlag,
    _Flags_Error_Kind_NoFlagPrefix,
    _Flags_Error_Kind_InvalidInt,
    _Flags_Error_Kind_UIntMinus,
    _Flags_Error_Kind_InvalidFloat,
    _Flags_Error_Kind_DuplicateFlag,
    _Flags_Error_Kind_RequireValue,
    _Flags_Error_Kind_SingleValue,
    _Flags_Error_Kind_InvalidBool,
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
        int64_t int_value;
        uint64_t uint_value;
        double float_value;
        Str8Array *str_value_arr;
        I64Array *int_value_arr;
        U64Array *uint_value_arr;
        F64Array *float_value_arr;
    } default_value;
    union
    {
        Str8 *str_value;
        bool *bool_value;
        int64_t *int_value;
        uint64_t *uint_value;
        double *float_value;
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
    uint64_t flags_index;
    _Flags_Error *first_error;
    _Flags_Error *last_error;
    uint64_t errors_index;
    Str8List error_list;
    bool has_error;
    Str8 program_name;
    bool should_add_color_flags;
    bool has_program_name;
    Log_Context log_context;
};

// Functions
// ============================================================================

internal Flags_Context flags_init(Alloc alloc);
internal void flags_add_color_flags(Flags_Context *context);
internal void flags_has_program_name(Flags_Context *context, bool has_name);
internal Str8 flags_get_program_name(Flags_Context *context);
internal bool flags_parse(Flags_Context *context, Str8Array *args);
internal void flags_print_error(Flags_Context *context);
internal void flags_print_help(Flags_Context *context);

// Add Flag ===================================================================

internal Flags_Flag *flags_string(Flags_Context *context, Str8 name, Str8 *result_value, Str8 default_value, Str8 description);
internal Flags_Flag *flags_int(Flags_Context *context, Str8 name, int64_t *result_value, int64_t default_value, Str8 description);
internal Flags_Flag *flags_uint(Flags_Context *context, Str8 name, uint64_t *result_value, uint64_t default_value, Str8 description);
internal Flags_Flag *flags_float(Flags_Context *context, Str8 name, double *result_value, double default_value, Str8 description);
internal Flags_Flag *flags_bool(Flags_Context *context, Str8 name, bool *result_value, bool default_value, Str8 description);
internal Flags_Flag *flags_str_arr(Flags_Context *context, Str8 name, Str8Array *result_value, Str8Array *default_value, Str8 description);
internal Flags_Flag *flags_int_arr(Flags_Context *context, Str8 name, I64Array *result_value, I64Array *default_value, Str8 description);
internal Flags_Flag *flags_uint_arr(Flags_Context *context, Str8 name, U64Array *result_value, U64Array *default_value, Str8 description);
internal Flags_Flag *flags_float_arr(Flags_Context *context, Str8 name, F64Array *result_value, F64Array *default_value, Str8 description);

// Flag setting ===============================================================

internal void flags_add_flag_shortname(Flags_Flag *flag, Str8 shortname);
internal void flags_make_flag_required(Flags_Flag *flag);
internal void flags_add_flag_value_hint(Flags_Flag *flag, Str8 value_hint);

// Private ====================================================================

internal Flags_Flag *_flags_get_flag(Flags_Context *context, Str8 name);
internal void _flags_add_flag(Flags_Context *context, Flags_Flag *flag);
internal void _flags_add_error(Flags_Context *context, _Flags_Error_Kind kind, Str8 name);
internal void _flags_add_error_with_value(Flags_Context *context, _Flags_Error_Kind kind, Str8 name, Str8 value);
internal Str8 _flags_get_options_from_arg(Str8 arg);
internal bool _flags_is_arg_option(Str8 arg);
internal uint64_t _flags_get_values_count(Str8Array *args, uint64_t index);

#endif // BASE_FLAGS_h
