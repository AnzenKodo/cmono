//~ ak: Private Types
//=============================================================================

typedef struct _Os_State _Os_State;
struct _Os_State
{
    Str8_Array args;
    Process_Info process_info;
    System_Info system_info;
};

//~ ak: Private Globals
//=============================================================================

global _Os_State _os_state = STRUCT_ZERO;

//~ ak: Os Core Functions
//=============================================================================

internal System_Info *os_system_info_get(void)
{
    return &_os_state.system_info;
}

//~ ak: Os Functions for Base Layer
//=============================================================================

//- ak: Processes =============================================================

internal Process_Info *process_info_get(void)
{
    return &_os_state.process_info;
}

internal Str8_Array *process_args_get(void)
{
    return &_os_state.args;
}

// ak: OS Entry Points
//=============================================================================

internal int32_t os_main(void)
{
    log_trace_g = log_alloc();
    int exit_code = base_main();
    log_free(log_trace_g);
    return exit_code;
}
