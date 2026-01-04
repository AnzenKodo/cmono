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

    // NOTE(aman.v): For software render
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;
    xcb_image_t *image;
    void *render_buffer;
};

// Macros
// ============================================================================

#define WlX11LoadAtom(name) \
    local_persist xcb_atom_t name = XCB_ATOM_NONE; \
    if (name == XCB_ATOM_NONE) { \
        Str8 atom_name = str8(#name); \
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(_wl_x11_state.connection, 0, atom_name.length, (char *)atom_name.cstr); \
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(_wl_x11_state.connection, cookie, NULL); \
        if (reply) { \
            name = reply->atom; \
            free(reply); \
        } else { \
            name = XCB_ATOM_NONE; \
            LogErrorLine(_os_core_state.log_context, "failed to load atom '"#name"'"); \
        } \
    }

// Global variables
//=============================================================================

global _Wl_X11_State _wl_x11_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_XCB_H
