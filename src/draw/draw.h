#ifndef DRAW_CORE_H
#define DRAW_CORE_H

// Types
// ============================================================================

typedef enum Draw_Type {
    Draw_Type_Rect,
} Draw_Type;

typedef struct Draw_Rect Draw_Rect;
struct Draw_Rect
{
    Vec4_F32 color;
    float    border_size;
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
    size_t    length;
};

// Functions
// ============================================================================

#endif // DRAW_CORE_H
