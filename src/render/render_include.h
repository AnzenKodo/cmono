#ifndef RENDER_INCLUDE_H
#define RENDER_INCLUDE_H

#include "render_core.h"

#if RENDER_BACKEND == RENDER_BACKEND_OPENGL
#   if OS_LINUX
#       include "render_egl.h"
#   elif OS_WINDOWS
#       include "render_wgl.h"
#   else
#       error no OpenGL render layer for this platform
#   endif
#   include "render_opengl.h"
#else
#   error no render layer for this platform
#endif

#endif // RENDER_INCLUDE_H
