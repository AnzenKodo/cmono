#ifndef WINDOW_LAYER_W32_H
#define WINDOW_LAYER_W32_H

// XCB Includes
//=============================================================================

// Types
//=============================================================================

typedef struct Wl_W32_State Wl_W32_State;
struct Wl_W32_State
{
    HINSTANCE instance;
};

// Global Variables
//=============================================================================

global Wl_W32_State wl_w32_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_W32_H
