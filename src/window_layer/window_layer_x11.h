#ifndef WINDOW_LAYER_XCB_H
#define WINDOW_LAYER_XCB_H

// External Includes
//=============================================================================

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

// Types
//=============================================================================

typedef struct _Wl_X11_State _Wl_X11_State;
struct _Wl_X11_State
{
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_atom_t wm_delete_window;
    xcb_atom_t wm_icon;

    // NOTE(aman.v): For software render
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;
    xcb_image_t *image;
    void *render_buffer;
};

// Global variables
//=============================================================================

global _Wl_X11_State _wl_x11_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_XCB_H
