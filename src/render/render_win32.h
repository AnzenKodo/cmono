#ifndef RENDER_X11_H
#define RENDER_X11_H

// Types
//=============================================================================

typedef struct Render_W32_State Render_W32_State;
struct Render_W32_State
{
    HBITMAP bitmap;
    BITMAPINFO bitmap_info;
    HDC hdc;
    PAINTSTRUCT paint;
    void *memory;
};

// Global Variables
//=============================================================================

global Render_W32_State render_w32_state = ZERO_STRUCT;

// Functions
//=============================================================================

#endif // RENDER_X11_H
