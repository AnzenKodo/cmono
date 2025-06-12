#if RENDER_BACKEND == RENDER_BACKEND_X11
#   include "render_x11.c"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL
#   include "render_opengl.c"
#else
#   error no render layer for this platform
#endif
