#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

// Types
//=============================================================================

typedef struct _Render_Opengl_State _Render_Opengl_State;
struct _Render_Opengl_State
{
    GLuint  vertex_arrays;
    GLuint  array_buffer;
    GLsizei array_buffer_size;
};

enum Render_Type
{
    Render_Type_Unsigend_Bype = GL_UNSIGNED_BYTE,
    Render_Type_Float         = GL_FLOAT,
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
    RenderOpenglXMacroWGL

#define X(name, r, p) typedef r name##_FunctionType p;
    RenderOpenglXMacro
#undef X
#define X(name, r, p) global name##_FunctionType *name = NULL;
    RenderOpenglXMacro
#undef X

// Functions
//=============================================================================

// OpenGL helper functions ====================================================

internal Void_Proc *_render_opengl_load_procedure(char *name);
internal uint32_t _render_opengl_shader_compile(char **source, uint32_t source_num, GLenum type, uint32_t program_id);

// Internal OpneGL functions ==================================================

internal void _render_opengl_init(void);
internal void _render_opengl_deinit(void);
internal void _render_opengl_begin(void);
internal void _render_opengl_end(void);

// Global variables
//=============================================================================

global _Render_Opengl_State _render_opengl_state = ZERO_STRUCT;

#endif // RENDER_OPENGL_H
