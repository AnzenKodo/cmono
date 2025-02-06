 #include "ccore/core.h"

int main(void)
{
    TCTX tctx;
    tctx_init_and_equip(&tctx);

    Os_Wl_Handle *os_wl_handle = os_window_open(str8_lit("Scuttle"), vec_2f32(400, 300));
    os_set_window_icon();

    Gfx_Canvas canvas = render_init();

    I16 x_offset = 0;
    I16 y_offset = 0;
    while (!os_should_window_close(os_wl_handle)) {
        os_wl_begin(os_wl_handle);
        if (os_is_key_pressed(os_wl_handle, Os_Key_Esc)) {
            os_set_window_close(os_wl_handle);
        }

        render_draw();
        gfx_create_square(canvas, (Rectangle){ 32, 32, 32, 32 }, (RGBA){ 100, 23, 53, 255 });
        ++x_offset;
        y_offset += 2;
    }

    render_close();
    os_window_close(os_wl_handle);
    return 0;
}
