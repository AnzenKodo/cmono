#ifndef RENDER_X11_H
#define RENDER_X11_H

// External Includes
//=============================================================================

#include <xcb/xcb_image.h>

// Types
//=============================================================================

typedef struct Render_X11_State Render_X11_State;
struct Render_X11_State
{
    xcb_pixmap_t pixmap;
    xcb_gcontext_t gc;
    xcb_image_t *image;
};

// Global Variables
//=============================================================================

global Render_X11_State render_x11_state = ZERO_STRUCT;

// Functions
//=============================================================================

#endif // RENDER_X11_H
