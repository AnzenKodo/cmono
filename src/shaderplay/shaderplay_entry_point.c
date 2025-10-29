#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../draw/draw.h"
#include "../render/render_include.h"
#include "../shaderplay/shaderplay_include.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../draw/draw.c"
#include "../render/render_include.c"
#include "../shaderplay/shaderplay_include.c"
#include <stdio.h>

#define FRAGMENT_SHADER_PATH   "shaders/circles.frag"

char *vert_source = "#version 330\n"
// "precision highp float;\n"
"in vec4 position;\n"
"void main() {\n"
"    gl_Position = position;\n"
"}\n";

internal void entry_point(void)
{
    fmt_printf("Hello");
}

internal void entry_point2(void)
{
    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), vec_2i32(750, 750));
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    render_init();

    // Load fragment shader file ==============================================
    Os_File frag_file = os_file_open(str8(FRAGMENT_SHADER_PATH), OS_AccessFlag_Read);
    Str8 frag_source = os_file_read_str_full(frag_file, alloc);
    U32 shader_id = render_shader_load(str8_from_cstr(vert_source), frag_source);

    // Get uniform locations
    U32 iTimeLocation = render_shader_get_value(shader_id, str8("iTime"));
    U32 iResolutionLocation = render_shader_get_value(shader_id, str8("iResolution"));

    // Set resolution (fixed window size)
    render_shader_set_value(iResolutionLocation, (float[2]){ (F32)wl_get_window_width(), (F32)wl_get_window_height() }, Render_Shader_Vec2);

    GLint positionAttrib = glGetAttribLocation(shader_id, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    U64 start = os_now_microsec();
    while (!wl_should_window_close())
    {
        wl_set_fps(60);
        wl_update_events();
        if (
            wl_is_key_pressed(Wl_Key_Esc) || wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        U64 now = os_now_microsec();
        float iTime = Cast(float)(now - start) / Million(1);
        render_shader_set_value(iTimeLocation, (float[1]){ iTime }, Render_Shader_Float);

        render_begin();
        {
            // glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        render_end();
    }

    // Free Everything ========================================================
    render_shader_unload(shader_id);
    render_deinit();
    wl_window_close();
    os_file_close(frag_file);
    os_memory_free(buffer, size);
}
