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

internal void entry_point(char *argv[])
{
    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), vec2i32(300, 300));
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    Draw_Buffer draw_buffer = render_init(alloc);

    draw_fill(draw_buffer, DRAW_RED);
    // Game Loop ==============================================================
    while (!wl_should_window_close()) {
        wl_update_events();
        if (
            wl_is_key_pressed(Wl_Key_Esc) ||
            wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        render_begin();
        {
            printf("hi");
        }
        render_end();
    }

    // Free Everything ========================================================
    render_deinit();
    os_memory_free(buffer, size);
    wl_window_close();
}
