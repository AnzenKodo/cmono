#ifndef BASE_FLAGS_H
#define BASE_FLAGS_H

// Types
// ============================================================================

typedef enum _Flags_Error_Kind
{
    _Flags_Error_Kind_MissingValue,
    _Flags_Error_Kind_UnknownOption,
    _Flags_Error_Kind_NoFlagPrefix,
    _Flags_Error_Kind_DuplicateOption,
    _Flags_Error_Kind_SingleValue,
    _Flags_Error_Kind_InvalidBool,
    _Flags_Error_Kind_OutIndexArg,
    // Option Enums
    _Flags_Error_Kind_RequireOption,
    _Flags_Error_Kind_InvalidIntOption,
    _Flags_Error_Kind_UIntMinusOption,
    _Flags_Error_Kind_InvalidFloatOption,
    // Arg Enums
    _Flags_Error_Kind_RequireArg,
    _Flags_Error_Kind_InvalidIntArg,
    _Flags_Error_Kind_UIntMinusArg,
    _Flags_Error_Kind_InvalidFloatArg,
} _Flags_Error_Kind;

typedef enum _Flags_Option_Kind
{
    _Flags_Option_Kind_Str,
    _Flags_Option_Kind_Int,
    _Flags_Option_Kind_UInt,
    _Flags_Option_Kind_Float,
    _Flags_Option_Kind_Bool,
    _Flags_Option_Kind_StrArr,
    _Flags_Option_Kind_IntArr,
    _Flags_Option_Kind_UIntArr,
    _Flags_Option_Kind_FloatArr,
} _Flags_Option_Kind;

typedef enum _Flags_Arg_Kind
{
    _Flags_Arg_Kind_Str,
    _Flags_Arg_Kind_Int,
    _Flags_Arg_Kind_UInt,
    _Flags_Arg_Kind_Float,
} _Flags_Arg_Kind;

typedef struct _Flags_Error _Flags_Error;
struct _Flags_Error
{
    _Flags_Error *next;
    _Flags_Error_Kind kind;
    Str8 flag_name;
    Str8 value;
    size_t arg_index;
};

typedef struct Flags_Option Flags_Option;
struct Flags_Option
{
    Flags_Option *next;
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
    _Flags_Option_Kind kind;
    bool assigned;
    bool required;
};

typedef struct Flags_Arg Flags_Arg;
struct Flags_Arg
{
    Flags_Arg *next;
    _Flags_Arg_Kind kind;
    size_t index;
    union
    {
        Str8 *str_value;
        bool *bool_value;
        int64_t *int_value;
        uint64_t *uint_value;
        double *float_value;
    } value;
    bool assigned;
    bool required;
};

typedef struct Flags_Context Flags_Context;
struct Flags_Context
{
    Alloc alloc;
    Flags_Option *first_option;
    Flags_Option *last_option;
    _Flags_Error *first_error;
    _Flags_Error *last_error;
    Flags_Arg *first_arg;
    Flags_Arg *last_arg;
    size_t index_arg;
    bool has_error;
    bool should_add_color_flags;
    bool has_program_name;
    Log_Context log_context;
};

// Functions
// ============================================================================

// Private functions ==========================================================

internal Flags_Option *_flags_get_options(Flags_Context *context, Str8 name);
internal void _flags_add_option(Flags_Context *context, Flags_Option *option);
internal Flags_Arg *_flags_get_arg(Flags_Context *context, size_t index);
internal void _flags_add_arg(Flags_Context *context, Flags_Arg *arg);
internal void _flags_add_option_error(Flags_Context *context, _Flags_Error_Kind kind, Str8 name);
internal void _flags_add_option_error_value(Flags_Context *context, _Flags_Error_Kind kind, Str8 name, Str8 value);
internal void _flags_add_error_arg(Flags_Context *context, _Flags_Error_Kind kind, size_t index, Str8 value);
internal bool _flags_is_arg_option(Str8 arg);
internal Str8 _flags_get_options_from_arg(Str8 arg);
internal uint64_t _flags_get_values_count(Str8Array *args, uint64_t index);

// Flags core functions =======================================================

internal Flags_Context flags_init(Alloc alloc);
internal bool flags_parse(Flags_Context *context, Str8Array *args);

internal void flags_print_error(Flags_Context *context);
internal void flags_print_help(Flags_Context *context);

// Flags config functions =====================================================

internal void flags_add_color_flags(Flags_Context *context);
internal void flags_has_program_name(Flags_Context *context, bool has_name);
internal void flags_add_option_shortname(Flags_Option *option, Str8 shortname);
internal void flags_add_option_value_hint(Flags_Option *option, Str8 value_hint);
internal void flags_make_option_required(Flags_Option *option);
internal void flags_make_arg_required(Flags_Arg *arg);

// Add option flag ============================================================

internal Flags_Option *flags_option_string(Flags_Context *context, Str8 name, Str8 *result_value, Str8 default_value, Str8 description);
internal Flags_Option *flags_option_int(Flags_Context *context, Str8 name, int64_t *result_value, int64_t default_value, Str8 description);
internal Flags_Option *flags_option_uint(Flags_Context *context, Str8 name, uint64_t *result_value, uint64_t default_value, Str8 description);
internal Flags_Option *flags_option_float(Flags_Context *context, Str8 name, double *result_value, double default_value, Str8 description);
internal Flags_Option *flags_option_bool(Flags_Context *context, Str8 name, bool *result_value, bool default_value, Str8 description);

internal Flags_Option *flags_str_arr(Flags_Context *context, Str8 name, Str8Array *result_value, Str8Array *default_value, Str8 description);
internal Flags_Option *flags_int_arr(Flags_Context *context, Str8 name, I64Array *result_value, I64Array *default_value, Str8 description);
internal Flags_Option *flags_uint_arr(Flags_Context *context, Str8 name, U64Array *result_value, U64Array *default_value, Str8 description);
internal Flags_Option *flags_float_arr(Flags_Context *context, Str8 name, F64Array *result_value, F64Array *default_value, Str8 description);

// Add value flag =============================================================

internal Flags_Arg *flags_arg_str(Flags_Context *context, Str8 *value);
internal Flags_Arg *flags_arg_int(Flags_Context *context, int64_t *value);
internal Flags_Arg *flags_arg_uint(Flags_Context *context, uint64_t *value);
internal Flags_Arg *flags_arg_float(Flags_Context *context, double *value);

#endif // BASE_FLAGS_h
