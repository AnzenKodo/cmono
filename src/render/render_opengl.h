#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

// Types
//=============================================================================

typedef struct Render_Opengl_State Render_Opengl_State;
struct Render_Opengl_State
{
    GLuint vertex_arrays;
    GLuint array_buffer;
    GLsizei array_buffer_size;
};

// Defines
//=============================================================================

#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

// X Macro
//=============================================================================

#define RenderOpenglXMacro \
    X(glGenBuffers, void, (GLsizei n, GLuint *buffers))\
    X(glBindBuffer, void, (GLenum target, GLuint buffer))\
    X(glDeleteBuffers, void, (GLsizei n, GLuint *buffers))\
    X(glGenVertexArrays, void, (GLsizei n, GLuint *arrays))\
    X(glBindVertexArray, void, (GLuint array))\
    X(glCreateProgram, GLuint, (void))\
    X(glCreateShader, GLuint, (GLenum type))\
    X(glShaderSource, void, (GLuint shader, GLsizei count, char **string, GLint *length))\
    X(glCompileShader, void, (GLuint shader))\
    X(glGetShaderiv, void, (GLuint shader, GLenum pname, GLint *params))\
    X(glGetShaderInfoLog, void, (GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog))\
    X(glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params))\
    X(glGetProgramInfoLog, void, (GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog))\
    X(glAttachShader, void, (GLuint program, GLuint shader))\
    X(glLinkProgram, void, (GLuint program))\
    X(glDeleteShader, void, (GLuint shader))\
    X(glUseProgram, void, (GLuint program))\
    X(glGetUniformLocation, GLint, (GLuint program, char *name))\
    X(glGetAttribLocation, GLint, (GLuint program, char *name))\
    X(glEnableVertexAttribArray, void, (GLuint index))\
    X(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer))\
    X(glBufferData, void, (GLenum target, ptrdiff_t size, void *data, GLenum usage))\
    X(glDeleteProgram, void, (GLuint program))\
    X(glDeleteVertexArrays, void, (GLsizei n, const GLuint *arrays))\
    X(glUniform1fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform2fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform3fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform4fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform1iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform2iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform3iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform4iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform1uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform2uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform3uv, void, (GLint location, GLsizei count, const GLuint *value))\
    X(glUniform4uv, void, (GLint location, GLsizei count, const GLuint *value))\

#define X(name, r, p) typedef r name##_FunctionType p;
    RenderOpenglXMacro
#undef X
#define X(name, r, p) global name##_FunctionType *name = NULL;
    RenderOpenglXMacro
#undef X

// Functions
//=============================================================================

// Internal functions =========================================================
internal void render_opengl_init(void);
internal void render_opengl_deinit(void);
internal void render_opengl_begin(void);
internal void render_opengl_end(void);

// Helper functions ===========================================================
internal Void_Proc *render_opengl_load_procedure(char *name);
internal U32 render_opengl_shader_compile(Str8 source, GLenum type, U32 program_id);

// Global Variables
//=============================================================================

global Render_Opengl_State render_opengl_state = ZERO_STRUCT;

#endif // RENDER_OPENGL_H
