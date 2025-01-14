#include "ccore/core.h"
#include "main.h"

typedef struct hello {
    U32 a;
    U32 b;
    U64 c;
} hello;

int main(void)
{
    Gfx_Handle *gfx_handle = gfx_win_open(str8_lit("Scuttle"), vec_2f32(400, 300));
    gfx_set_icon();
    Render_Backbuffer buffer = render_init();

    // XEvent event;
    I16 x_offset = 0;
    I16 y_offset = 0;
    while (!gfx_should_win_close(gfx_handle)) {
        gfx_begin(gfx_handle);
        if (gfx_is_key_pressed(gfx_handle, Gfx_Key_Esc)) {
            gfx_set_win_close(gfx_handle);
        }

        render_draw();
        ui_gradiant_grid(buffer, x_offset, y_offset);
        ui_square(buffer, (Rectangle){ 32, 32, 32, 32 }, (RGBA){ 255, 0, 0, 255 }, 30, 32);
        ++x_offset;
        y_offset += 2;
    }

    render_close();
    gfx_win_close(gfx_handle);

    return 0;
}
