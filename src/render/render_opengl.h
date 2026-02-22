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
}
_Render_Opengl_Vertex_Loc;

typedef struct _Render_Opengl_Tex2D _Render_Opengl_Tex2D;
struct _Render_Opengl_Tex2D
{
    GLuint id;
    Render_Resource_Kind resource_kind;
    Render_Tex2D_Format format;
    size_t width;
    size_t height;
};

typedef struct _Render_Opengl_FormatInfo _Render_Opengl_FormatInfo;
struct _Render_Opengl_FormatInfo
{
    GLint internal_format;
    GLenum format;
    GLenum base_type;
};

typedef struct _Render_Opengl_State _Render_Opengl_State;
struct _Render_Opengl_State
{
    GLuint shader;
    GLuint vao;
    GLuint vbo;
    GLuint default_texture;
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
    X(glDebugMessageCallback, void, (void (*)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void *userParam), const void *userParam))\
    RenderOpenglXMacroWGL

#define X(name, r, p) typedef r name##_FunctionType p;
    RenderOpenglXMacro
#undef X
#define X(name, r, p) global name##_FunctionType *name = NULL;
    RenderOpenglXMacro
#undef X

// Defines
// ============================================================================

// NOTE(ak): below constants list taken from: [GL Constants Translator](https://javagl.github.io/GLConstantsTranslator/GLConstantsTranslator.html)

#define GL_DEBUG_OUTPUT                 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS     0x8242
#define GL_DEBUG_SEVERITY_HIGH          0x9146
#define GL_DEBUG_SEVERITY_MEDIUM        0x9147
#define GL_DEBUG_SEVERITY_LOW           0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION  0x826B

#define GL_COMPILE_STATUS               0x8B81

#define GL_FRAGMENT_SHADER              0x8B30
#define GL_VERTEX_SHADER                0x8B31

#define GL_ARRAY_BUFFER                 0x8892
#define GL_STATIC_DRAW                  0x88E4

#define GL_RG                           0x8227

// Functions
//=============================================================================

// OpenGL helper functions ====================================================

//~ ak: General relper functions
internal Void_Proc *_render_opengl_load_procedure(char *name);
internal void _render_opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void *userParam);
//~ ak: Shader helper functions
internal uint32_t _render_opengl_shader_compile(char **source, uint32_t source_num, GLenum type, uint32_t program_id);
internal uint32_t _render_opengl_shader_load(char **vert_sources, uint32_t vert_source_num, char **frag_sources, uint32_t frag_source_num);
//~ ak: Texture helper functions
internal Render_Handle _render_handle_from_opengl_tex2d(_Render_Opengl_Tex2D *tex2d);
internal _Render_Opengl_Tex2D *_render_opengl_tex2d_from_handle(Render_Handle handle);
internal _Render_Opengl_FormatInfo _render_opengl_format_info_from_tex2d_format(Render_Tex2D_Format format);

// Internal OpneGL functions ==================================================

internal void _render_opengl_init(void);
internal void _render_opengl_deinit(void);
internal void _render_opengl(void);

// Global variables
//=============================================================================

global _Render_Opengl_State _render_opengl_state = ZERO_STRUCT;

#endif // RENDER_OPENGL_H
