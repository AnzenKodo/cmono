#define STB_TRUETYPE_IMPLEMENTATION
#include "../font/external/stb_truetype.h"

#define KB_TEXT_SHAPE_IMPLEMENTATION
#include "../font/external/kb_text_shape.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned char* load_file(const char* path, size_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *out_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buffer = (unsigned char*)malloc(*out_size);
    if (buffer) fread(buffer, 1, *out_size, f);
    fclose(f);
    return buffer;
}

int main(int argc, char** argv) {
    const char* font_path = "./assets/font/VendSans-Regular.ttf";
    const char* text = (argc >= 3) ? argv[2] : "Hello World(:-)^_^)";  // Text with potential ligatures (fi, fl, etc.)

    size_t ttf_size = 0;
    unsigned char* ttf_buffer = load_file(font_path, &ttf_size);
    if (!ttf_buffer) {
        printf("Failed to load font file\n");
        return 1;
    }

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, ttf_buffer, 0)) {
        printf("Failed to initialize font\n");
        free(ttf_buffer);
        return 1;
    }

    float font_pixel_height = 96.0f;
    float scale = stbtt_ScaleForPixelHeight(&info, font_pixel_height);

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);

    const int img_w = 1280;
    const int img_h = 300;
    unsigned char* rgb = (unsigned char*)calloc(img_w * img_h * 3, 1);  // White background (0xff would be white, but calloc gives 0; we'll treat as white)

    // Fill with white
    memset(rgb, 255, img_w * img_h * 3);

    kbts_shape_context* ctx = kbts_CreateShapeContext(0, 0);  // Default allocator (malloc/free)

    kbts_ShapePushFontFromMemory(ctx, ttf_buffer, (int)ttf_size, 0);

    kbts_ShapeBegin(ctx, KBTS_DIRECTION_LTR, KBTS_LANGUAGE_DONT_KNOW);
    kbts_ShapeUtf8(ctx, text, (int)strlen(text), KBTS_USER_ID_GENERATION_MODE_CODEPOINT_INDEX);
    kbts_ShapeEnd(ctx);

    // Compute total width for centering
    float total_width = 0.0f;
    kbts_run run;
    while (kbts_ShapeRun(ctx, &run)) {
        kbts_glyph* glyph;
        kbts_glyph_iterator iter = run.Glyphs;
        while (kbts_GlyphIteratorNext(&iter, &glyph)) {
            total_width += (float)glyph->AdvanceX * scale;
        }
    }

    float start_x = (img_w - total_width) / 2.0f;
    float pos_x = start_x;
    float baseline_y = img_h / 2.0f + (ascent * scale);

    // Second pass: render (reshape to reset runs)
    kbts_ShapeBegin(ctx, KBTS_DIRECTION_LTR, KBTS_LANGUAGE_DONT_KNOW);
    kbts_ShapeUtf8(ctx, text, (int)strlen(text), KBTS_USER_ID_GENERATION_MODE_CODEPOINT_INDEX);
    kbts_ShapeEnd(ctx);

    while (kbts_ShapeRun(ctx, &run)) {
        kbts_glyph* glyph;
        kbts_glyph_iterator iter = run.Glyphs;
        while (kbts_GlyphIteratorNext(&iter, &glyph)) {
            int gid = glyph->Id;
            if (gid == 0) continue;  // Missing glyph

            float gx = pos_x + (float)glyph->OffsetX * scale;
            float gy = baseline_y + (float)glyph->OffsetY * scale;

            int gw, gh, gxoff, gyoff;
            unsigned char* bitmap = stbtt_GetGlyphBitmap(&info, scale, scale, gid, &gw, &gh, &gxoff, &gyoff);

            if (bitmap && gw > 0 && gh > 0) {
                for (int y = 0; y < gh; ++y) {
                    for (int x = 0; x < gw; ++x) {
                        int px = (int)floorf(gx + gxoff + x + 0.5f);
                        int py = (int)floorf(gy + gyoff + y + 0.5f);
                        if (px >= 0 && px < img_w && py >= 0 && py < img_h) {
                            unsigned char alpha = bitmap[y * gw + x];
                            if (alpha > 0) {
                                float a = alpha / 255.0f;
                                int idx = (py * img_w + px) * 3;
                                for (int c = 0; c < 3; ++c) {
                                    rgb[idx + c] = (unsigned char)(rgb[idx + c] * (1.0f - a));
                                }
                            }
                        }
                    }
                }
                stbtt_FreeBitmap(bitmap, NULL);
            }

            pos_x += (float)glyph->AdvanceX * scale;
        }
    }

    // Write PPM (P6 binary)
    FILE* out = fopen("./build/img.ppm", "wb");
    if (out) {
        fprintf(out, "P6\n%d %d\n255\n", img_w, img_h);
        fwrite(rgb, 1, img_w * img_h * 3, out);
        fclose(out);
        printf("Rendered text to output.ppm\n");
    } else {
        printf("Failed to write output.ppm\n");
    }

    free(rgb);
    free(ttf_buffer);
    kbts_DestroyShapeContext(ctx);

    return 0;
}

#if 0
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../font/font.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../font/font.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* load_file(const char* filename, int* out_size) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buffer = (unsigned char*)malloc(sz);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, sz, f);
    fclose(f);
    if (out_size) *out_size = (int)sz;
    return buffer;
}

void save_ppm(const char* filename, const unsigned char* pixels, int w, int h) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Cannot write to file '%s'\n", filename);
        return;
    }
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; ++i) {
        unsigned char val = 255 - pixels[i];  // Invert: black text on white background
        fputc(val, f);
        fputc(val, f);
        fputc(val, f);
    }
    fclose(f);
    printf("Saved font atlas to '%s'\n", filename);
}

void entry_point(void) {
    const char* ttf_file = "./assets/font/VendSans-Regular.ttf";
    const char* ppm_file = "./build/img.ppm";
    float pixel_height = 48;
    int bitmap_size = 400;

    if (pixel_height <= 0 || bitmap_size <= 0 || bitmap_size > 16384) {
        printf("Error: Invalid pixel_height or bitmap_size\n");
        // return 1;
    }

    int ttf_size;
    unsigned char* ttf_buffer = load_file(ttf_file, &ttf_size);
    if (!ttf_buffer) {
        // return 1;
    }

    unsigned char* bitmap = (unsigned char*)calloc(bitmap_size * bitmap_size, 1);
    if (!bitmap) {
        printf("Error: Out of memory\n");
        free(ttf_buffer);
        // return 1;
    }

    stbtt_bakedchar cdata[96];  // For characters 32 to 127
    int res = stbtt_BakeFontBitmap(ttf_buffer, 0, pixel_height, bitmap,
                                   bitmap_size, bitmap_size,
                                   32, 96, cdata);

    free(ttf_buffer);

    if (res <= 0) {
        printf("Error: Font baking failed.\n");
        if (res < 0) {
            printf("Only %d characters fitted before running out of space.\n", -res);
            printf("Try a larger bitmap_size.\n");
        }
        free(bitmap);
        // return 1;
    }

    printf("Success: All 96 characters baked, using %d pixel rows.\n", res);
    save_ppm(ppm_file, bitmap, bitmap_size, bitmap_size);
    free(bitmap);

    // return 0;
}
#endif
