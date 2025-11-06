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

internal void entry_point(void)
{
    char *vert_source = "in vec4 position;\n"
        "void main() {\n"
        "    gl_Position = position;\n"
        "}\n";

    char *shader_init_code = "out vec4 fragColor;\n"
        "uniform float iTime;\n"
        "uniform vec2  iResolution;\n"
        "uniform float iTimeDelta;\n"
        "uniform float iFrame;\n"
        "uniform float iChannelTime[4];\n"
        "uniform vec4  iMouse;\n"
        "uniform vec4  iDate;\n"
        "uniform float iSampleRate;\n"
        "uniform vec3  iChannelResolution[4];\n"
        "uniform float iChannel0;\n"
        "uniform float iChannel1;\n"
        "uniform float iChannel2;\n"
        "uniform float iChannel3;\n"
        "void mainImage(out vec4 fragColor, in vec2 fragCoord);\n"
        "vec2 mainSound(float time);\n"
        "void mainVR(out vec4 fragColor, in vec2 fragCoord, in vec3 fragRayOri, in vec3 fragRayDir);\n"
        "void main() {\n"
        "   mainImage(fragColor, gl_FragCoord.xy);\n"
        // "   mainSound(time);\n"
        // "   mainVR(fragColor, gl_FragCoord.xy, in vec3 fragRayOri, in vec3 fragRayDir);\n"
        "}\n";

    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), vec_2i32(750, 750));
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    render_init();

    U32 shader_id, i_time, i_resolution, i_time_delta, i_frame, i_channel_time, i_mouse, i_date, i_samplerate, i_channel_resolution, i_channel_0, i_channel_1, i_channel_2, i_channel_3;
    DenseTime old_modified = 0;

    U64 start = os_now_microsec();
    while (!wl_should_window_close())
    {
        Os_File frag_file = os_file_open(str8(FRAGMENT_SHADER_PATH), OS_AccessFlag_Read|OS_AccessFlag_ShareWrite);
        Os_FileProperties prop = os_file_properties(frag_file);
        if (prop.modified && prop.modified != old_modified)
        {
            old_modified = prop.modified;
            Str8 frag_source = os_file_read_str_full(frag_file, alloc);
            char* vert_sources[] = {
                shader_source_header,
                vert_source,
            };
            char* frag_sources[] = {
                shader_source_header,
                shader_init_code,
                (char *)frag_source.cstr
            };
            shader_id = render_shader_load_multi(vert_sources, 2, frag_sources, 3);

            i_time               = render_shader_get_value(shader_id, str8("iTime"));
            i_resolution         = render_shader_get_value(shader_id, str8("iResolution"));
            i_time_delta         = render_shader_get_value(shader_id, str8("iTimeDelta"));
            i_frame              = render_shader_get_value(shader_id, str8("iFrame"));
            i_channel_time       = render_shader_get_value(shader_id, str8("iChannelTime"));
            i_mouse              = render_shader_get_value(shader_id, str8("iMouse"));
            i_date               = render_shader_get_value(shader_id, str8("iDate"));
            i_samplerate         = render_shader_get_value(shader_id, str8("iSampleRate"));
            i_channel_resolution = render_shader_get_value(shader_id, str8("iChannelResolution"));
            i_channel_0          = render_shader_get_value(shader_id, str8("iChannel0"));
            i_channel_1          = render_shader_get_value(shader_id, str8("iChannel1"));
            i_channel_2          = render_shader_get_value(shader_id, str8("iChannel2"));
            i_channel_3          = render_shader_get_value(shader_id, str8("iChannel3"));

            GLint positionAttrib = glGetAttribLocation(shader_id, "position");
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
        render_shader_set_value(i_resolution, (float[2]){ (F32)wl_get_window_width(), (F32)wl_get_window_height() }, Render_Shader_Vec2);

        wl_set_fps(60);
        wl_update_events();
        if (
            wl_is_key_pressed(Wl_Key_Esc) || wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        U64 now = os_now_microsec();
        float iTime = Cast(float)(now - start) / Million(1);
        render_shader_set_value(i_time, (float[1]){ iTime }, Render_Shader_Float);

        render_begin();
        {
            // glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        render_end();
    }

    // Free Everything ========================================================
    render_deinit();
    wl_window_close();
    // os_file_close(frag_file);
    os_memory_free(buffer, size);
}
