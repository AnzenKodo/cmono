#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../draw/draw_include.h"
#include "../render/render_include.h"
#include "../scuttle/scuttle_include.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../draw/draw_include.c"
#include "../render/render_include.c"
#include "../scuttle/scuttle_include.c"

internal void
gen_next(I32 *tilemap, I32 width, I32 height)
{
    for (I32 w = 0; w < width; w++)
    {
        for (I32 h = 0; h < height; h++)
        {
            I32 alive_count = 0;
            for (I32 k = -1; k <= 1; k++) {
                for (I32 l = -1; l <= 1; l++) {
                    if (k == 0 && l == 0) continue;
                    if (w + k < height && w + k >= 0 && h + l < width && h + l >= 0) {
                        if (tilemap[(w+k)*width+(h+l)] == 1) {
                            alive_count++;
                        }
                    }
                }
            }

            switch (alive_count) {
                case 0:
                case 1: {
                    tilemap[w*width+h] = 0;
                    break;
                }
                case 2:
                case 3: {
                    if (tilemap[w*width+h] == 0 && alive_count == 3) {
                        tilemap[w*width+h] = 1;
                    }
                    break;
                }
                default: {
                    tilemap[w*width+h] = 0;
                    break;
                }
            }
        } // for h
    } // for w
}

internal void
entry_point(char *argv[])
{
    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), vec2i32(750, 750));
    wl_window_icon_set(cast(U32 *)ICON, ICON_WIDTH, ICON_HEIGHT);
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    Draw_Buffer draw_buffer = render_init(alloc);

    // Tilemap Init ===========================================================
    U32 tile_height = 30;
    U32 tile_width = 30;
    I32 tilemap[TILEMAP_COUNT_Y][TILEMAP_COUNT_X];
    mem_set(tilemap, 0, sizeof(tilemap));
    for (I32 i = 0; i < TILEMAP_COUNT_X/5; i++)
    {
        for (I32 j = 0; j < TILEMAP_COUNT_X/5; j++)
        {
            tilemap[i][j] = 1;
        }
    }
    while (!wl_should_window_close())
    {
        wl_set_fps(60);
        wl_update_events();
        if (
            wl_is_key_pressed(Wl_Key_Esc) ||
            wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        render_begin();
        {
            // Draw Tile Loop =================================================
            gen_next(&tilemap, TILEMAP_COUNT_Y, TILEMAP_COUNT_X);
            for (I32 row = 0; row < TILEMAP_COUNT_Y; ++row) {
                for (I32 col = 0; col < TILEMAP_COUNT_X; ++col) {
                    Draw_Rgba color = DRAW_BLUE;
                    if (tilemap[row][col] == 1) {
                        color = DRAW_YELLOW;
                    }

                    F32 rect_x = cast(F32)col * tile_width;
                    F32 rect_y = cast(F32)row * tile_height;
                    F32 rect_width  = rect_x + tile_width;
                    F32 rect_height = rect_y + tile_height;
                    draw_rect(draw_buffer, (Draw_Rect){
                        rect_x, rect_y, rect_width, rect_height
                    }, color);
                } // for col
            } // for row
        }
        render_end();
    }

    // Free Everything ========================================================
    render_deinit();
    os_memory_free(buffer, size);
    wl_window_close();
}
