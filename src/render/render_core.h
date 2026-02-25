#ifndef RENDER_CORE_H
#define RENDER_CORE_H

// Types
//=============================================================================

typedef struct Render_Handle Render_Handle;
struct Render_Handle
{
    uint64_t u64[1];
};

typedef enum Render_Resource_Kind
{
    Render_Resource_Kind_Static,
    Render_Resource_Kind_Dynamic,
    Render_Resource_Kind_Stream,
    Render_Resource_Kind_COUNT,
}
Render_Resource_Kind;

typedef enum Render_Tex2D_Format
{
    Render_Tex2D_Format_R8,
    Render_Tex2D_Format_RG8,
    Render_Tex2D_Format_RGBA8,
    Render_Tex2D_Format_BGRA8,
    Render_Tex2D_Format_R16,
    Render_Tex2D_Format_RGBA16,
    Render_Tex2D_Format_R32,
    Render_Tex2D_Format_RG32,
    Render_Tex2D_Format_RGBA32,
}
Render_Tex2D_Format;

//~ ak: Draw Types ============================================================

typedef enum Render_Draw_Type {
    Render_Draw_Type_Rect,
} Render_Draw_Type;

typedef struct Render_Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Render_Color;

typedef struct Render_Draw_Rect Render_Draw_Rect;
struct Render_Draw_Rect
{
    float         border_size;
    Vec4_F32      color;
    Vec4_F32      dst;
    Vec4_F32      src;
    Vec4_F32      border_color;
    Render_Handle texture;
};

typedef struct Render_Draw_Node Render_Draw_Node;
struct Render_Draw_Node
{
    Render_Draw_Node *next;
    Render_Draw_Type type;
    union {
        Render_Draw_Rect param_rect;
    };
};

typedef struct Render_Draw_List Render_Draw_List;
struct Render_Draw_List
{
    Render_Draw_Node *first;
    Render_Draw_Node *last;
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


//- ak: Functions
//=============================================================================

//~ ak: Helper functions ======================================================

internal Render_Handle render_handle_zero(void);
inline internal Render_Color render_hex_to_color(unsigned int hex);
inline internal unsigned int render_color_to_hex(Render_Color color);
inline internal Vec4_F32 render_color_to_vec4_f32(Render_Color color);

//- ak: Core functions ========================================================

internal void render_init(void);
internal void render(Render_Draw_List *list);
internal void render_deinit(void);

//- ak: Texture functions =====================================================

internal Render_Handle render_tex2d_alloc(Render_Resource_Kind kind, Render_Tex2D_Format format, size_t width, size_t height, void *data, Arena *arena);
internal void render_tex2d_free(Render_Handle handle);

//- ak: Draw functions ========================================================

internal void render_draw_rect_push(Arena *arena, Render_Draw_List *list, Vec4_F32 dst, Render_Color color);

#endif // RENDER_CORE_H
