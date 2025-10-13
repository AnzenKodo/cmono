#include "render_core.c"

#if RENDER_BACKEND == RENDER_BACKEND_OPENGL
#   if OS_LINUX
#       include "render_opengl_linux.h"
#   elif OS_WINDOWS
#       include "render_opengl_win32.h"
#   else
#       error no OpenGL render layer for this platform
#   endif
#else
#   error no render layer for this platform
#endif
