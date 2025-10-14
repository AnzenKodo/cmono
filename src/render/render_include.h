#ifndef RENDER_INCLUDE_H
#define RENDER_INCLUDE_H

#include "render_core.h"

#if RENDER_BACKEND == RENDER_BACKEND_EGL
#   if OS_LINUX
#       include "render_egl_linux.h"
#   elif OS_WINDOWS
#       include "render_egl_win32.h"
#   else
#       error no OpenGL render layer for this platform
#   endif
#else
#   error no render layer for this platform
#endif

#endif // RENDER_INCLUDE_H
