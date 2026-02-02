// External Includes
//=============================================================================

#define STB_TRUETYPE_IMPLEMENTATION
typedef uint8_t  stbtt_uint8;
typedef int8_t   stbtt_int8;
typedef uint16_t stbtt_uint16;
typedef int16_t  stbtt_int16;
typedef uint32_t stbtt_uint32;
typedef int32_t  stbtt_int32;
#define STBTT_ifloor(x)   ((int) floor_f64(x))
#define STBTT_iceil(x)    ((int) ceil_f64(x))
#define STBTT_sqrt(x)      sqrt_f64(x)
#define STBTT_pow(x,y)     pow_f64(x,y)
#define STBTT_fmod(x,y)    fmod_f64(x,y)
#define STBTT_cos(x)       cos_f64(x)
#define STBTT_acos(x)      acos_f64(x)
#define STBTT_fabs(x)      Abs(x)
#define STBTT_malloc(x,u)  ((void)(u),malloc(x))
#define STBTT_free(x,u)    ((void)(u),free(x))
#define STBTT_assert(x)    Assert(x)
#define STBTT_strlen(x)    cstr8_length((uint8_t* )x)
#define STBTT_memcpy       mem_copy
#define STBTT_memset       mem_set
#include "../external/stb_truetype.h"

internal void draw_rect_push(Arena *arena, Draw_List *list, Vec4_F32 dst, Vec4_F32 color)
{
    Unused(dst);
    Draw_Node *node = arena_push(arena, Draw_Node, 1);
    node->type = Draw_Type_Rect;
    Draw_Rect *rect = &node->param_rect;
    rect->dst = dst;
    rect->color = color;
    SLLQueuePush(list->first, list->last, node);
}
