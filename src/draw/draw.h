#ifndef DRAW_H
#define DRAW_H

// Types
// ============================================================================

typedef enum Draw_Corner
{
    Draw_Corner_TopLeft = 1,
    Draw_Corner_BottomLeft,
    Draw_Corner_TopRight,
    Draw_Corner_BottomRight,
    Draw_Corner_COUNT
} Draw_Corner;

typedef enum Draw_Type {
    Draw_Type_Rect,
} Draw_Type;

typedef struct Draw_Rect Draw_Rect;
struct Draw_Rect
{
    Vec4_F32 color;
    Vec4_F32 dst;
    Vec4_F32 border_color;
    float    border_size;
    uint32_t texture;
};

typedef struct Draw_Node Draw_Node;
struct Draw_Node
{
    Draw_Node *next;
    Draw_Type type;
    union {
        Draw_Rect param_rect;
    };
};

typedef struct Draw_List Draw_List;
struct Draw_List
{
    Draw_Node *first;
    Draw_Node *last;
};

// Functions
// ============================================================================

#endif // DRAW_H
