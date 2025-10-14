#include "render_core.c"

#if RENDER_BACKEND == RENDER_BACKEND_EGL
#   include "render_egl_core.h"
#   if OS_LINUX
#       include "render_egl_linux.h"
#   elif OS_WINDOWS
#       include "render_opengl_win32.h"
#   else
#       error no OpenGL render layer for this platform
#   endif
#else
#   error no render layer for this platform
#endif
