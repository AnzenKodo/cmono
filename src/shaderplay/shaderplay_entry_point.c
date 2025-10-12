#define RENDER_BACKEND RENDER_BACKEND_OPENGL_WIN32

#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../draw/draw_include.h"
#include "../render/render_include.h"
#include "../shaderplay/shaderplay_include.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../draw/draw_include.c"
#include "../render/render_include.c"
#include "../shaderplay/shaderplay_include.c"
#include <stdio.h>

internal void
entry_point(void)
{
    // Program Init ===========================================================
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    wl_window_open(str8("Scuttle"), vec_2i32(750, 750));
    Draw_Buffer draw_buffer = render_init(alloc);
    // wl_window_icon_set(cast(U32 *)ICON, ICON_WIDTH, ICON_HEIGHT);

    // Load fragment shader file ==============================================
    while (!wl_should_window_close()) {
        wl_update_events();
        if (wl_is_key_pressed(Wl_Key_Esc) ||
            wl_is_event_happen(Wl_EventType_WindowClose))
        {
            wl_set_window_close();
        }
        render_begin();
        {
            draw_fill(draw_buffer, DRAW_RED);
        }
        render_end();
    }
    // Str8List *args = os_args_get();
    // Str8Node *frag_filename = args->first->next;
    // Os_File file = os_file_open(frag_filename->string, OS_AccessFlag_Read);
    // Str8 fragmentShaderSource = os_file_read_str_full(file, alloc);

    // Free Everything ========================================================
    // os_file_close(file);
    os_memory_free(buffer, size);
    wl_window_close();
}

char *vertexShaderSource = "#version 300 es\n"
"precision highp float;\n"
"in vec4 position;\n"
"void main() {\n"
"    gl_Position = position;\n"
"}\n";

// internal void
// entry_point(void)
// {
//     // Program Init ===========================================================
//     wl_window_open(str8("Scuttle"), vec2i32(750, 750));
//     // wl_window_icon_set(cast(U32 *)ICON, ICON_WIDTH, ICON_HEIGHT);
//     U64 size = MB(10);
//     void *buffer = os_memory_alloc(size);
//     Alloc alloc = alloc_arena_init(buffer, size);
//     Draw_Buffer draw_buffer = render_init(alloc);
//
//     // Load fragment shader file ==============================================
//     Str8List *args = os_args_get();
//     Str8Node *frag_filename = args->first->next;
//     Os_File file = os_file_open(frag_filename->string, OS_AccessFlag_Read);
//     Str8 fragmentShaderSource = os_file_read_str_full(file, alloc);
//
//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//     glCompileShader(vertexShader);
//     GLint status;
//     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
//     if (!status) {
//         char log[512];
//         glGetShaderInfoLog(vertexShader, 512, NULL, log);
//         printf("Shader program linking failed: %s", log);
//         return;
//     }
//
//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource.str, NULL);
//     glCompileShader(fragmentShader);
//     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
//     if (!status) {
//         char log[512];
//         glGetShaderInfoLog(fragmentShader, 512, NULL, log);
//         printf("Shader program linking failed: %s", log);
//         return;
//     }
//
//     GLuint program = glCreateProgram();
//     glAttachShader(program, vertexShader);
//     glAttachShader(program, fragmentShader);
//     glLinkProgram(program);
//     glGetProgramiv(program, GL_LINK_STATUS, &status);
//     if (!status) {
//         char log[512];
//         glGetProgramInfoLog(program, 512, NULL, log);
//         printf("Shader program linking failed: %s", log);
//         return;
//     }
//     glUseProgram(program);
//
//     // Get uniform locations
//     GLint iTimeLocation = glGetUniformLocation(program, "iTime");
//     GLint iResolutionLocation = glGetUniformLocation(program, "iResolution");
//
//     // Set resolution (fixed window size)
//     glUniform2f(iResolutionLocation, 800.0f, 600.0f);
//
//     // Sed
//     GLfloat vertices[] = {
//         -1.0f, -1.0f,
//          1.0f, -1.0f,
//         -1.0f,  1.0f,
//          1.0f,  1.0f
//     };
//
//     GLuint vbo;
//     glGenBuffers(1, &vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//     GLint positionAttrib = glGetAttribLocation(program, "position");
//     glEnableVertexAttribArray(positionAttrib);
//     glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//     U64 start = os_now_microsec();
//     while (!wl_should_window_close())
//     {
//         wl_set_fps(60);
//         wl_update_events();
//         if (
//             wl_is_key_pressed(Wl_Key_Esc) || wl_is_event_happen(Wl_EventType_WindowClose)
//         ) {
//             wl_set_window_close();
//         }
//
//         U64 now = os_now_microsec();
//         float iTime = cast(float)(now - start) / Million(1);
//         glUniform1f(iTimeLocation, iTime);
//
//         render_begin();
//         {
//             glClear(GL_COLOR_BUFFER_BIT);
//             glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//         }
//         render_end();
//     }
//
//     // Free Everything ========================================================
//
//     glDeleteProgram(program);
//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);
//     glDeleteBuffers(1, &vbo);
//     os_file_close(file);
//     render_deinit();
//     os_memory_free(buffer, size);
//     wl_window_close();
// }
