#include "ccore/base/base_include.h"
#include "ccore/platform/platform_include.h"
#include "ccore/core_draw.h"

#include "ccore/base/base_include.c"
#include "ccore/platform/platform_include.c"
#include "ccore/core_img.h"

fn void shape_gradient_grid(
    Draw_Buffer buffer, I16 blue_offset, I16 green_offset
) {
    I16 bytes_per_pixel = buffer.bytes_per_pixel;
    I16 height = buffer.height;
    I16 width = buffer.width;

    I32 pitch = width * bytes_per_pixel;
    U8 *row = cast(U8 *)buffer.memory;
    for (int y = 0; y < height; y++)
    {
        U32 *pixel = cast(U32 *)row;
        for (int x = 0; x < width; x++) {
            U8 blue = (x + blue_offset);
            U8 green = (y + green_offset);
            *pixel++ = ((green << 8) | blue);
        }
        row += pitch;
    }
}

fn void shape_rect_pattern(
    Draw_Buffer buffer,
    I32 cols, I32 rows,
    F32 fill_width, F32 fill_height,
    Rgba color1, Rgba color2
) {
    F32 cell_width = fill_width/cols;
    F32 cell_height = fill_height/rows;
    for (I32 y = 0; y < rows; ++y)
    {
        for (I32 x = 0; x < cols; ++x)
        {
            Rgba color;
            if ((x + y)%2 == 0) {
                color = color1;
            } else {
                color = color2;
            }
            draw_rect(buffer, (Rect){ x*cell_width, y*cell_height, cell_width, cell_height}, color);
        }
    }
}

int main(void) {
    TCTX tctx;
    tctx_init_and_equip(&tctx);

    Draw_Buffer buffer = draw_create_canvas(300, 300, 4);
    draw_fill_canvas(buffer, GRAY);
    shape_rect_pattern(buffer, 6, 6, buffer.width, buffer.height, LIGHTGRAY, PINK);
    draw_rect(buffer, draw_center_rect(buffer, 100, 200), BEIGE);
    // draw_create_circle(buffer, draw_center_rect(buffer, 100, 200), 32, BLUE);
    // draw_circle(buffer, 300/2, 300/2, 100, BLUE);
    draw_circle2(buffer, 300/2, 300/2, 100, RED);

    ppm_save(buffer, str8_lit("./img.ppm"));

    draw_free_canvas();
    tctx_release();

    return 0;
}
