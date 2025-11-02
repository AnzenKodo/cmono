#ifndef OS_STUB_H
#define OS_STUB_H

// External Includes
//=============================================================================

// Types
//=============================================================================

typedef struct Os_Stub_State Os_Stub_State;
struct Os_Stub_State {
};

// Global Variables
//=============================================================================

global Os_Stub_State _wl_stub_state = ZERO_STRUCT;

#endif // OS_STUB_H
