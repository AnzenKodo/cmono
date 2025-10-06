#if RENDER_BACKEND == RENDER_BACKEND_X11
#   include "render_x11.c"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL
#   include "render_opengl.c"
#elif RENDER_BACKEND == RENDER_BACKEND_WIN32
#   include "render_win32.c"
#else
#   error no render layer for this platform
#endif
