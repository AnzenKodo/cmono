#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// Types
//=============================================================================

struct Render_Vertex Render_Vertex;
struct Render_Vertex
{
    Vec2_F32 offset;
    Vec2_F32 size;
    Vec4_F32 color;
};

// Define Render Backends
//=============================================================================

#define RENDER_BACKEND_STUB   0
#define RENDER_BACKEND_OPENGL 1

// Context detection for Render Backends
//=============================================================================

#ifndef RENDER_BACKEND
#   if OS_LINUX
#        define RENDER_BACKEND RENDER_BACKEND_OPENGL
#   elif OS_WINDOWS
#        define RENDER_BACKEND RENDER_BACKEND_OPENGL
#   endif
#endif


// Functions
//=============================================================================

// Core functions =============================================================
internal void render_init(void);
internal void render(Draw_List *list);
internal void render_deinit(void);

#endif // RENDER_CORE_H
