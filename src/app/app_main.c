// TODO(ak): add multi-window support
// TODO(ak): update event handling
// TODO(ak): make flags state based
// TODO(ak): make fmt printf's internal
// TODO(ak): add freelist in render texture

//- ak: headers
#include "./app.h"
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../font/font.h"
#include "../ui/ui_include.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../font/font.c"
#include "../render/render_include.c"
#include "../ui/ui_include.c"

internal void base_main(void)
{
    Arena *arena = arena_alloc();
    
    // Program Init ===========================================================
    unsigned int width = 150;
    unsigned int height = 300;
    wl_window_open(str8(APP_NAME), width, height);
    wl_window_border_set(false);
    wl_window_pos_set(wl_display_width_get()-width-30, 0);
    render_init();
    
    // Program Loop ===========================================================
    while (!wl_should_window_close())
    {
        render_begin();
        wl_set_fps(60);
        wl_update_events();
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose))
        {
            wl_set_window_close();
        }
        // render_draw_rect_push(box->rect, (Render_Color){ 212, 190, 152, 255 });
        render_end();
    }
    
    // Free Everything ========================================================
    render_deinit();
    wl_window_close();
}
