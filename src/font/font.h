// TODO(ak): add unicode support
// TODO(ak): store font as cpu image
#ifndef FONT_H
#define FONT_H

// External Includes
//=============================================================================

#define STBTT_STATIC
#include "./external/stb_truetype.h"

#define KB_TEXT_SHAPE_STATIC
#define kbts_u64 uint64_t
#define kbts_u32 uint32_t
#define kbts_u16 uint16_t
#define kbts_u8  uint8_t
#define kbts_s64 int64_t
#define kbts_s32 int32_t
#define kbts_s16 int16_t
#define kbts_s8  int8_t
#define kbts_b32 int
#include "kb_text_shape.h"

// Types
//=============================================================================

typedef struct Font Font;
struct Font
{
    stbtt_packedchar data[256];
    void *pixels;
    size_t size;
    float height;
    float scale;
    float line_height;
    size_t atlas_width;
    size_t atlas_height;
    Render_Handle texture;
};

// Defines
//=============================================================================

#define FONT_SIZE_TTF_DEFAULT 32

// Functions
//=============================================================================

internal Font font_load(Str8 name, size_t atlas_width, size_t atlas_height, Arena *arena);
internal void font_unload(Font *font);
internal void font_quad_push(Font *font, Str8 text, Vec2_F32 pos, Render_Color color, Render_Draw_List *list, Arena *arena);

#endif // FONT_H
