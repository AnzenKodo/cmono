#ifndef RENDER_WGL_H
#define RENDER_WGL_H

// External Includes
//=============================================================================

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdi32.lib")
#include <GL/gl.h>

// Defines
//=============================================================================

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094

// X Macro
//=============================================================================

#define RenderOpenglXMacroWGL \
    X(glGenBuffers, void, (GLsizei n, GLuint *buffers))\
    X(glBindBuffer, void, (GLenum target, GLuint buffer))\
    X(glDeleteBuffers, void, (GLsizei n, GLuint *buffers))\
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
    X(glUniform1fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform2fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform3fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform4fv, void, (GLint location, GLsizei count, const GLfloat *value))\
    X(glUniform1iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform2iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform3iv, void, (GLint location, GLsizei count, const GLint *value))\
    X(glUniform4iv, void, (GLint location, GLsizei count, const GLint *value))\

// OpenGL setup function pointers
//=============================================================================

typedef BOOL WINAPI FNWGLCHOOSEPIXELFORMATARBPROC(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC WINAPI FNWGLCREATECONTEXTATTRIBSARBPROC(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL WINAPI FNWGLSWAPINTERVALEXTPROC(int interval);

// Types
//=============================================================================

typedef struct _Render_Wgl_State _Render_Wgl_State;
struct _Render_Wgl_State
{
    HDC hdc;
    HGLRC contex;
};

// Global Variables
//=============================================================================

global _Render_Wgl_State _render_wgl_state = ZERO_STRUCT;
global char* shader_source_header = "#version 330\n";

// Functions
//=============================================================================

#endif // RENDER_WGL_H
