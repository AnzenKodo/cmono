#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

// External Includes
//=============================================================================

#include <GL/gl.h>

// Types
//=============================================================================

typedef enum _Render_Opengl_Vertex_Loc
{
    _Render_Opengl_Vertex_Loc_Pos = 0,
} _Render_Opengl_Vertex_Loc;

typedef struct _Render_Opengl_State _Render_Opengl_State;
struct _Render_Opengl_State
{
    GLuint shader;
    GLuint vao;
    GLuint vbo;
};

// X Macro
//=============================================================================

#ifndef RenderOpenglXMacroWGL
#   define RenderOpenglXMacroWGL
#endif

#define RenderOpenglXMacro \
    X(glGenVertexArrays, void, (GLsizei n, GLuint *arrays))\
    X(glBindVertexArray, void, (GLuint array))\
    X(glDeleteVertexArrays, void, (GLsizei n, const GLuint *arrays))\
    X(glUniform1uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform2uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform3uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform4uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glDebugMessageControl, void, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled))\
    X(glDebugMessageCallback, void, (void (*)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam), void *user_data))\
    RenderOpenglXMacroWGL

#define X(name, r, p) typedef r name##_FunctionType p;
    RenderOpenglXMacro
#undef X
#define X(name, r, p) global name##_FunctionType *name = NULL;
    RenderOpenglXMacro
#undef X

// Defines
// ============================================================================

#define GL_DEBUG_OUTPUT                 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS     0x8242
#define GL_DEBUG_SEVERITY_HIGH          0x9146
#define GL_DEBUG_SEVERITY_MEDIUM        0x9147
#define GL_DEBUG_SEVERITY_LOW           0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION  0x826B

// Functions
//=============================================================================

// OpenGL helper functions ====================================================

internal Void_Proc *_render_opengl_load_procedure(char *name);
internal uint32_t _render_opengl_shader_compile(char **source, uint32_t source_num, GLenum type, uint32_t program_id);
internal uint32_t _render_opengl_shader_load(char **vert_sources, uint32_t vert_source_num, char **frag_sources, uint32_t frag_source_num);
internal void _render_opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

// Internal OpneGL functions ==================================================

internal void _render_opengl_init(void);
internal void _render_opengl_deinit(void);
internal void _render_opengl(void);

// Global variables
//=============================================================================

global _Render_Opengl_State _render_opengl_state = ZERO_STRUCT;

#endif // RENDER_OPENGL_H
