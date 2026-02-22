// TODO(ak): add unicode support
// TODO(ak): store font as cpu image
#ifndef FONT_H
#define FONT_H

// External Includes
//=============================================================================

#define STBTT_STATIC
#include "./external/stb_truetype.h"

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
internal void font_quad_push(Font *font, Str8 text, Vec2_F32 pos, Vec4_F32 color, Render_Draw_List *list, Arena *arena);

#endif // FONT_H
