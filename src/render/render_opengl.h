#ifndef RENDER_OPENGL_H
#define RENDER_OPENGL_H

// ak: External Includes
//=============================================================================

#include <GL/gl.h>

// ak: Types
//=============================================================================

typedef enum _Render_Opengl_Vertex_Loc
{
    _Render_Opengl_Vertex_Loc_Pos = 0,
}
_Render_Opengl_Vertex_Loc;

typedef enum _Render_Opengl_Shader_Kind
{
    _Render_Opengl_Shader_Kind_Rect,
    _Render_Opengl_Shader_Kind_COUNT,
}
_Render_Opengl_Shader_Kind;

typedef struct _Render_Opengl_Attribute _Render_Opengl_Attribute;
struct _Render_Opengl_Attribute
{
    uint64_t index;
    Str8 name;
    GLenum type;
    uint64_t count;
};

typedef struct _Render_Opengl_Attribute_Array _Render_Opengl_Attribute_Array;
struct _Render_Opengl_Attribute_Array
{
    _Render_Opengl_Attribute *v;
    uint64_t count;
};

typedef struct _Render_Opengl_FormatInfo _Render_Opengl_FormatInfo;
struct _Render_Opengl_FormatInfo
{
    GLint internal_format;
    GLenum format;
    GLenum base_type;
};

typedef struct _Render_Opengl_RenderTarget _Render_Opengl_RenderTarget;
struct _Render_Opengl_RenderTarget
{
    GLuint fbo;
    GLuint color_texture;
};

typedef struct _Render_Opengl_Window _Render_Opengl_Window;
struct _Render_Opengl_Window
{
    _Render_Opengl_Window *next;
    _Render_Opengl_Window *prev;
    Render_Handle window_os;
    Vec2_F32 last_canvas_rect_dim;
    _Render_Opengl_RenderTarget stage_target;
    _Render_Opengl_RenderTarget stage_scratch_target;
};

typedef struct _Render_Opengl_Tex2D _Render_Opengl_Tex2D;
struct _Render_Opengl_Tex2D
{
    _Render_Opengl_Tex2D *next;
    _Render_Opengl_Tex2D *prev;
    GLuint id;
    Render_Resource_Kind resource_kind;
    Render_Tex2D_Format format;
    Vec2_I32 size;
};

typedef struct _Render_Opengl_State _Render_Opengl_State;
struct _Render_Opengl_State
{
    Arena *arena;
    size_t render_begin_arena_pos;
    GLuint shaders[_Render_Opengl_Shader_Kind_COUNT];
    GLuint vao;
    GLuint vbo;
    GLuint white_texture;
    _Render_Opengl_Window *free_window;
    _Render_Opengl_Tex2D *free_tex2d;
};

typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

// ak: X Macro
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
    X(glBlitFramebuffer, void, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter))\
    X(glBindFragDataLocation, void, (GLuint program, GLuint color, char *name))\
    X(glDrawArraysInstanced, void, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount))\
    X(glVertexAttribDivisor, void, (GLuint index, GLuint divisor))\
    RenderOpenglXMacroWGL

#define X(name, r, p) typedef r name##_FunctionType p;
    RenderOpenglXMacro
#undef X
#define X(name, r, p) global name##_FunctionType *name = NULL;
    RenderOpenglXMacro
#undef X

// ak: Defines
//=============================================================================

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

    
#define glUseProgramScope(...)              DeferLoop(glUseProgram(__VA_ARGS__), glUseProgram(0))
#define glBindVertexArrayScope(...)         DeferLoop(glBindVertexArray(__VA_ARGS__), glBindVertexArray(0))
#define glBindFramebufferScope(target, ...) DeferLoop(glBindFramebuffer((target), __VA_ARGS__), glBindFramebuffer((target), 0))
#define glBindTextureScope(target, ...)     DeferLoop(glBindTexture((target), __VA_ARGS__), glBindTexture((target), 0))
#define glEnableScope(...)                  DeferLoop(glEnable(__VA_ARGS__), glDisable(__VA_ARGS__))
    
// ak: Functions
//=============================================================================

// ak: OpenGL helper functions ================================================

// ak: General helper functions
internal Void_Proc *_render_opengl_load_procedure(char *name);
// ak: Texture helper functions
internal Render_Handle _render_handle_from_opengl_tex2d(_Render_Opengl_Tex2D *tex2d);
internal _Render_Opengl_Tex2D *_render_opengl_tex2d_from_handle(Render_Handle handle);
internal _Render_Opengl_FormatInfo _render_opengl_format_info_from_tex2d_format(Render_Tex2D_Format format);
internal void _render_opengl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

// ak: Internal OpneGL functions ==================================================

internal void _render_opengl_init(void);
internal Render_Handle _render_opengl_window_equip(Wl_Window window);
internal void _render_opengl_window_unequip(Wl_Window window, Render_Handle handle);
internal void _render_opengl_select_window(Wl_Window window, Render_Handle r);
internal void _render_opengl_window_swap(Wl_Window window, Render_Handle r);

// ak: Global variables
//=============================================================================

global _Render_Opengl_State *_render_opengl_state = ZERO_STRUCT;
read_only global Str8 _render_opengl_shader_rect_vert_src;
read_only global Str8 _render_opengl_shader_rect_frag_src;
extern Str8 *_render_opengl_shader_kind_vert_src_table[_Render_Opengl_Shader_Kind_COUNT];
extern Str8 *_render_opengl_shader_kind_frag_src_table[_Render_Opengl_Shader_Kind_COUNT];
extern _Render_Opengl_Attribute_Array _render_opengl_shader_kind_input_attributes_table[_Render_Opengl_Shader_Kind_COUNT];
extern _Render_Opengl_Attribute_Array _render_opengl_shader_kind_output_attributes_table[_Render_Opengl_Shader_Kind_COUNT];

#endif // RENDER_OPENGL_H
