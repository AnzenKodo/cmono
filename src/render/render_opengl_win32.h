#ifndef RENDER_OPENGL_WIN32_H
#define RENDER_OPENGL_WIN32_H

// External Includes
//=============================================================================

#include <GL/gl.h>

// Types
//=============================================================================

typedef struct Render_Opengl_Win32_State Render_Opengl_Win32_State;
struct Render_Opengl_Win32_State
{
    HBITMAP bitmap;
    BITMAPINFO bitmap_info;
    HDC hdc;
    PAINTSTRUCT paint;
    void *memory;
    GLuint tex_handle;
};

// Global Variables
//=============================================================================

global Render_Opengl_Win32_State render_opengl_win32_state = ZERO_STRUCT;

// Functions
//=============================================================================

#endif // RENDER_OPENGL_WIN32_H
