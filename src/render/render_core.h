#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// Define Render Backends
//=============================================================================

#define RENDER_BACKEND_STUB     0
#define RENDER_BACKEND_X11      1
#define RENDER_BACKEND_OPENGL   2

// Context detection for Render Backends
//=============================================================================

#ifndef RENDER_BACKEND
#   if OS_LINUX
#        define RENDER_BACKEND RENDER_BACKEND_X11
#   endif
#endif

// Functions
//=============================================================================

internal Draw_Buffer render_init(Alloc);
internal void render_begin(void);
internal void render_end(void);
internal void render_deinit(void);

#endif // RENDER_CORE_H
