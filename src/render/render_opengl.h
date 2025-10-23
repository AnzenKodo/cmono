#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

#define GL_COMPILE_STATUS                 0x8B81
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4

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
    X(glValidateProgram, void, (GLuint program))\
    X(glDeleteShader, void, (GLuint shader))\
    X(glUseProgram, void, (GLuint program))\
    X(glGetUniformLocation, GLint, (GLuint program, char *name))\
    X(glGetAttribLocation, GLint, (GLuint program, char *name))\
    X(glEnableVertexAttribArray, void, (GLuint index))\
    X(glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer))\
    X(glBufferData, void, (GLenum target, ptrdiff_t size, void *data, GLenum usage))\
    X(glBufferSubData, void, (GLenum target, ptrdiff_t offset, ptrdiff_t size, const void *data))\
    X(glBlendFuncSeparate, void, (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha))\
    X(glUniform1f, void, (GLint location, GLfloat v0))\
    X(glUniform2f, void, (GLint location, GLfloat v0, GLfloat v1))\
    X(glUniform3f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2))\
    X(glUniform4f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3))\
    X(glUniformMatrix4fv, void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value))\
    X(glUniform1i, void, (GLint location, GLint v0))\
    X(glTexImage3D, void, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels))\
    X(glTexSubImage3D, void, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels))\
    X(glGenerateMipmap, void, (GLenum target))\
    X(glBindAttribLocation, void, (GLuint programObj, GLuint index, char *name))\
    X(glBindFragDataLocation, void, (GLuint program, GLuint color, char *name))\
    X(glActiveTexture, void, (GLenum texture))\
    X(glVertexAttribDivisor, void, (GLuint index, GLuint divisor))\
    X(glDrawArraysInstanced, void, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount))\
    X(glDeleteProgram, void, (GLuint program))\

internal Void_Proc *render_opengl_load_procedure(char *name);

internal void render_init(void);
internal void render_deinit(void);
internal void render_begin(void);
internal void render_end(void);

#endif // RENDER_OPENGL_H
