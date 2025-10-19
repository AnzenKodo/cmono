#include "render_core.c"

#if RENDER_BACKEND == RENDER_BACKEND_EGL
#   include "render_egl_core.c"
#   if OS_LINUX
#       include "render_egl_linux.c"
#   elif OS_WINDOWS
#       include "render_egl_win32.c"
#   else
#       error no OpenGL render layer for this platform
#   endif
#else
#   error no render layer for this platform
#endif
