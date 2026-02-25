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
#include "./external/stb_truetype.h"

internal Font font_load(Str8 name, size_t atlas_width, size_t atlas_height, Arena *arena)
{
    Font font = ZERO_STRUCT;
    font.atlas_width = atlas_width;
    font.atlas_height = atlas_height;
    font.pixels = arena_push(arena, unsigned char, atlas_width * atlas_height);
    font.size = FONT_SIZE_TTF_DEFAULT;
    // ak: Load font file
    Arena_Temp temp = arena_temp_begin(arena);
    {
        unsigned char *ttf_buffer = arena_push(temp.arena, unsigned char, MB(1));
        Os_File font_file = os_file_open(name, OS_AccessFlag_Read);
        os_file_read_full(font_file, ttf_buffer);
        // ak: init font data
        stbtt_fontinfo font_info;
        stbtt_InitFont(&font_info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
        //~ ak: compute scale and metrics *before* packing
        font.scale = stbtt_ScaleForPixelHeight(&font_info, 32.0f);
        int ascent, descent, line_gap;
        stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
        font.line_height = (ascent - descent + line_gap) * font.scale;
        font.height = (ascent - descent) * font.scale;
        stbtt_pack_context pc;
        stbtt_PackBegin(&pc, font.pixels, atlas_width, atlas_height, 0, 1, NULL);
        {
            stbtt_PackFontRange(&pc, ttf_buffer, 0, font.size, 32, 224, font.data);
        }
        stbtt_PackEnd(&pc);
    }
    arena_temp_end(temp);
    font.texture = render_tex2d_alloc(Render_Resource_Kind_Dynamic, Render_Tex2D_Format_R8, atlas_width, atlas_height, font.pixels, arena);
    return font;
}

internal void font_unload(Font *font)
{
   render_tex2d_free(font->texture);
}

internal void font_quad_push(Font *font, Str8 text, Vec2_F32 pos, Render_Color color, Render_Draw_List *list, Arena *arena)
{
    float x = pos.x;
    float y = pos.y+(font->height/2);
    for (char *c = (char *)text.cstr; *c; ++c)
    {
        if (*c < 32 || *c > 126)
        {
            if (*c == '\n')
            {
                y += font->line_height;
                x = pos.x;
            }
        }
        else
        {
            stbtt_aligned_quad q;
            stbtt_GetPackedQuad(font->data, font->atlas_width, font->atlas_height, *c - 32, &x, &y, &q, true);
            Render_Draw_Node *node = arena_push(arena, Render_Draw_Node, 1);
            Render_Draw_Rect *rect = &node->param_rect;
            rect->dst = (Vec4_F32){ q.x0, q.y0, q.x1-q.x0, q.y1-q.y0 };
            rect->src = (Vec4_F32){ q.s0, q.t0, q.s1-q.s0, q.t1-q.t0 };
            rect->texture = font->texture;
            rect->color = render_color_to_vec4_f32(color);
            SLLQueuePush(list->first, list->last, node);
        }
    }
}
