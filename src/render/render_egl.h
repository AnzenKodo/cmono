#ifndef RENDER_EGL_H
#define RENDER_EGL_H

// External Includes
//=============================================================================

#include <EGL/egl.h>
#include <GLES2/gl2.h>

// Types
//=============================================================================

typedef struct _Render_Egl_State _Render_Egl_State;
struct _Render_Egl_State
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

// Global Variables
//=============================================================================

global _Render_Egl_State _render_egl_state = ZERO_STRUCT;
global char* shader_source_header = "#version 300 es\nprecision highp float;\n";

#endif // RENDER_EGL_H
