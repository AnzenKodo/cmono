#ifndef RENDER_INCLUDE_H
#define RENDER_INCLUDE_H

#include "render_core.h"

#if RENDER_BACKEND == RENDER_BACKEND_X11
#   include "render_x11.h"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL
#   include "render_opengl.h"
#elif RENDER_BACKEND == RENDER_BACKEND_WIN32
#   include "render_win32.h"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL_WIN32
#   include "render_opengl_win32.h"
#else
#   error no render layer for this platform
#endif

#endif // RENDER_INCLUDE_H
