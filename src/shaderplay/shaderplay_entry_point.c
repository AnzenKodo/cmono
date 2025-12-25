#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "./shaderplay_core.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../render/render_include.c"

#define VERSION "0.1"

internal void print_help_message(Flags_Context *context)
{
    term_style_start(TERM_UNDERLINE);
    fmt_println("USAGE:");
    term_style_end();
    Str8 *program_name = os_program_path_get();
    fmt_printfln("   %s [OPTIONS] <shader_file_name.frag>", program_name->cstr);
    term_style_start(TERM_UNDERLINE);
    fmt_println("OPTIONS:");
    term_style_end();
    flags_print_help(context);
    term_style_start(TERM_UNDERLINE);
    fmt_println("VERSION:");
    term_style_end();
    fmt_println("   "VERSION);
}

internal void entry_point(void)
{
    uint64_t size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);

    // Command Line ===========================================================
    Flags_Context context = flags_init(alloc);
    Flags_Option *option = NULL;
    Flags_Arg *farg = NULL;

    Str8 frag_filepath;
    farg = flags_arg_str(&context, &frag_filepath);
    flags_make_arg_required(farg);
    uint64_t fps = 60;
    flags_option_uint(&context, str8("fps"), &fps, fps, str8("Set Framws-Per-Seconds for window"));
    uint64_t height = 500;
    option = flags_option_uint(&context, str8("height"), &fps, fps, str8("Set Framws-Per-Seconds for window"));
    flags_add_option_shortname(option, str8("h"));
    flags_add_option_value_hint(option, str8("<HEIGHT>"));
    uint64_t width = 750;
    option = flags_option_uint(&context, str8("width"), &fps, fps, str8("Set Framws-Per-Seconds for window"));
    flags_add_option_shortname(option, str8("w"));
    flags_add_option_value_hint(option, str8("<WIDTH>"));
    bool no_aot = false;
    flags_option_bool(&context, str8("no-aot"), &no_aot, no_aot, str8("Disable window Always-On-Top"));
    bool help = false;
    option = flags_option_bool(&context, str8("help"), &help, help, str8("Print help message"));
    flags_add_option_shortname(option, str8("h"));
    bool version = false;
    option = flags_option_bool(&context, str8("version"), &version, version, str8("Print version message"));
    flags_add_option_shortname(option, str8("v"));
    flags_add_color_flags(&context);

    Str8Array *args = os_args_get();
    if (!flags_parse(&context, args))
    {
        flags_print_error(&context);
        print_help_message(&context);
        os_exit(1);
    }
    if (help)
    {
        print_help_message(&context);
        os_exit(0);
    }
    if (version)
    {
        fmt_print("v"VERSION);
        os_exit(0);
    }

    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), vec2_i32(width, height));
    render_init();

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

    uint32_t shader_id, i_time, i_resolution;
    // uint32_t i_time_delta, i_frame, i_channel_time, i_mouse, i_date, i_samplerate, i_channel_resolution, i_channel_0, i_channel_1, i_channel_2, i_channel_3;
    DenseTime old_modified = 0;
    Os_File frag_file;

    // Program Loop ===========================================================
    uint64_t start = os_now_microsec();
    while (!wl_should_window_close())
    {
        frag_file = os_file_open(frag_filepath, OS_AccessFlag_Read|OS_AccessFlag_ShareWrite);
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
            if (shader_id)
            {
                render_shader_unload(shader_id);
            }
            shader_id = render_shader_load_multi(vert_sources, 2, frag_sources, 3);

            i_time               = render_shader_get_value(shader_id, str8("iTime"));
            i_resolution         = render_shader_get_value(shader_id, str8("iResolution"));
            // i_time_delta         = render_shader_get_value(shader_id, str8("iTimeDelta"));
            // i_frame              = render_shader_get_value(shader_id, str8("iFrame"));
            // i_channel_time       = render_shader_get_value(shader_id, str8("iChannelTime"));
            // i_mouse              = render_shader_get_value(shader_id, str8("iMouse"));
            // i_date               = render_shader_get_value(shader_id, str8("iDate"));
            // i_samplerate         = render_shader_get_value(shader_id, str8("iSampleRate"));
            // i_channel_resolution = render_shader_get_value(shader_id, str8("iChannelResolution"));
            // i_channel_0          = render_shader_get_value(shader_id, str8("iChannel0"));
            // i_channel_1          = render_shader_get_value(shader_id, str8("iChannel1"));
            // i_channel_2          = render_shader_get_value(shader_id, str8("iChannel2"));
            // i_channel_3          = render_shader_get_value(shader_id, str8("iChannel3"));
            GLint positionAttrib = glGetAttribLocation(shader_id, "position");
            glEnableVertexAttribArray(positionAttrib);
            glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
        render_shader_set_value(i_resolution, (float[2]){ (float)wl_get_window_width(), (float)wl_get_window_height() }, Render_Shader_Vec2);

        wl_set_fps(fps);
        wl_update_events();
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        uint64_t now = os_now_microsec();
        float iTime = (float)(now - start) / Million(1);
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
    os_file_close(frag_file);
    os_memory_free(buffer, size);
}
