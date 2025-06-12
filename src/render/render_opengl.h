#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>

typedef struct Render_Opengl_State Render_Opengl_State;
struct Render_Opengl_State
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
};

// Global Variables
//=============================================================================

global Render_Opengl_State render_opengl_state = ZERO_STRUCT;

#endif // RENDER_OPENGL_H
