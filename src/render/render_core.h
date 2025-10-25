#ifndef RENDER_CORE_H
#define RENDER_CORE_H

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


typedef enum {
    Render_Shader_Float = 0,       // Shader uniform type: float
    Render_Shader_Vec2,            // Shader uniform type: vec2 (2 float)
    Render_Shader_Vec3,            // Shader uniform type: vec3 (3 float)
    Render_Shader_Vec4,            // Shader uniform type: vec4 (4 float)
    Render_Shader_Int,             // Shader uniform type: int
    Render_Shader_Ivec2,           // Shader uniform type: ivec2 (2 int)
    Render_Shader_Ivec3,           // Shader uniform type: ivec3 (3 int)
    Render_Shader_Ivec4,           // Shader uniform type: ivec4 (4 int)
    Render_Shader_Uint,            // Shader uniform type: unsigned int
    Render_Shader_Uivec2,          // Shader uniform type: uivec2 (2 unsigned int)
    Render_Shader_Uivec3,          // Shader uniform type: uivec3 (3 unsigned int)
    Render_Shader_Uivec4,          // Shader uniform type: uivec4 (4 unsigned int)
} Render_Shader;

// Functions
//=============================================================================

internal void render_init(void);
internal void render_begin(void);
internal void render_end(void);
internal void render_deinit(void);
internal U32 render_shader_load(Str8 vert_source, Str8 frag_source);
internal void render_shader_unload(U32 id);

#endif // RENDER_CORE_H
