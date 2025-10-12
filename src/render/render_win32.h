#ifndef RENDER_WIN32_H
#define RENDER_WIN32_H

// Types
//=============================================================================

typedef struct Render_Win32_State Render_Win32_State;
struct Render_Win32_State
{
    HBITMAP bitmap;
    BITMAPINFO bitmap_info;
    HDC hdc;
    PAINTSTRUCT paint;
    void *memory;
};

// Global Variables
//=============================================================================

global Render_Win32_State render_win32_state = ZERO_STRUCT;

// Functions
//=============================================================================

#endif // RENDER_WIN32_H
