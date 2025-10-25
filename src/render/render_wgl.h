#ifndef RENDER_WGL_H
#define RENDER_WGL_H

// External Includes
//=============================================================================

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#include <GL/gl.h>

// Defines
//=============================================================================

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094

// OpenGL setup function pointers
//=============================================================================

typedef BOOL WINAPI FNWGLCHOOSEPIXELFORMATARBPROC(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC WINAPI FNWGLCREATECONTEXTATTRIBSARBPROC(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL WINAPI FNWGLSWAPINTERVALEXTPROC(int interval);

// Types
//=============================================================================

typedef struct Render_Wgl_State Render_Wgl_State;
struct Render_Wgl_State
{
    HDC hdc;
    HGLRC contex;
};

// Global Variables
//=============================================================================

global Render_Wgl_State render_wgl_state = ZERO_STRUCT;

// Functions
//=============================================================================

#endif // RENDER_WGL_H
