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

internal Font font_load(Str8 name, float font_size, unsigned char atlas_width, unsigned char atlas_height, Arena *arena)
{
    Font font = ZERO_STRUCT;
    font.atlas_width = atlas_width;
    font.atlas_height = atlas_height;
    font.pixels = arena_push(arena, unsigned char, atlas_width * atlas_height);
    // ak: Load font file
    Arena_Temp temp = arena_temp_begin(arena);
    unsigned char *ttf_buffer = arena_push(temp.arena, unsigned char, MB(1));
    Os_File font_file = os_file_open(name, OS_AccessFlag_Read);
    os_file_read_full(font_file, ttf_buffer);
    // ak: init font data
    stbtt_fontinfo font_info;
    stbtt_InitFont(&font_info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, font.pixels, atlas_width, atlas_height, 0, 1, NULL);
    {
        stbtt_PackFontRange(&pc, ttf_buffer, 0, font_size, 32, 224, font.data);
    }
    stbtt_PackEnd(&pc);
    arena_temp_end(temp);
    return font;
}

// internal void font_write_letter(Font *font, char letter, float x, float y)
// {
//     stbtt_aligned_quad q;
//     stbtt_GetPackedQuad(font->data, font->atlas_width, font->atlas_height, letter - 32, &x, &y, &q, true);
// }
//
// typedef struct
// {
//    float x0,y0,s0,t0; // top-left
//    float x1,y1,s1,t1; // bottom-right
// } stbtt_aligned_quad;
//
// internal void font_write(Font *font, char letter, float x, float y)
// {
//     Vec4_F32 result[4] = ZERO_STRUCT;
//     stbtt_aligned_quad q;
//     stbtt_GetPackedQuad(font->data, font->atlas_width, font->atlas_height, letter - 32, &x, &y, &q, true);
//     glTexCoord2f(q.s0, q.t0, q.x0, q.y0);
//     glTexCoord2f(q.s1, q.t0, q.x1, q.y0);
//     glTexCoord2f(q.s1, q.t1, q.x1, q.y1);
//     glTexCoord2f(q.s0, q.t1, q.x0, q.y1);
//     return (Vec4_F32)q;
// }
