#include "window_layer_core.c"

#if OS_LINUX
#   include "window_layer_x11.c"
#else
#   error OS window layer not implemented for this operating system.
#endif
