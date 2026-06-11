#ifndef WINDOW_LAYER_XCB_H
#define WINDOW_LAYER_XCB_H

// ak: External Includes
//=============================================================================

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>

// ak: Types
//=============================================================================

typedef struct _Wl_X11_Window _Wl_X11_Window;
struct _Wl_X11_Window
{
    _Wl_X11_Window *next;
    _Wl_X11_Window *prev;
    xcb_window_t xwindow;
    xcb_image_t *image;
    void *buffer;
    Rng2_F32 rect;
    Rng2_F32 canvas_rect;
};

typedef struct _Wl_X11_State _Wl_X11_State;
struct _Wl_X11_State
{
    _Wl_X11_Window *free_window;
    _Wl_X11_Window *first_window;
    _Wl_X11_Window *last_window;
    Arena *arena;
    
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_atom_t wm_delete_window;
    xcb_atom_t wm_protocols;
    
    // Software Render
    xcb_pixmap_t pixmap;
    xcb_format_t *pixmap_format;
    xcb_gcontext_t gc;
};

// ak: Macros
//=============================================================================

#define WlX11LoadAtom(state, name) \
    xcb_atom_t name = XCB_ATOM_NONE; \
    if (name == XCB_ATOM_NONE) { \
        Str8 atom_name = str8(#name); \
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(state->connection, 0, atom_name.length, (char *)atom_name.cstr); \
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(state->connection, cookie, NULL); \
        if (reply) { \
            name = reply->atom; \
            free(reply); \
        } else { \
            name = XCB_ATOM_NONE; \
            LogErrorLine(&_os_core_state.log_context, "failed to load atom '"#name"'"); \
        } \
    }


// ak: Helper functions
//=============================================================================

internal _Wl_X11_Window *_wl_x11_window_from_xwindow();

// ak: Global Variables
//=============================================================================

global _Wl_X11_State *_wl_x11_state = NULL;

#endif // WINDOW_LAYER_XCB_H
