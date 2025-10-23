#ifndef RENDER_EGL_H
#define RENDER_EGL_H

// External Includes
//=============================================================================

#include <EGL/egl.h>
#include <GLES2/gl2.h>

// Types
//=============================================================================

typedef struct Render_Egl_State Render_Egl_State;
struct Render_Egl_State
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

// Global Variables
//=============================================================================

global Render_Egl_State render_egl_state = ZERO_STRUCT;

#endif // RENDER_EGL_H
