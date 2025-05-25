#ifndef WINDOW_LAYER_LINUX_H
#define WINDOW_LAYER_LINUX_H

// XCB Includes
//=============================================================================

#include <xcb/xcb.h>

// Types
//=============================================================================

typedef struct Wl_Linux_State Wl_Linux_State;
struct Wl_Linux_State {
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    xcb_intern_atom_reply_t wm_delete_window;
    xcb_window_t window;
};

// Global Variables
//=============================================================================

global Wl_Linux_State wl_linux_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_LINUX_H
