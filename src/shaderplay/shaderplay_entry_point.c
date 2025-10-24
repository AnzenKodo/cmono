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
    GLint iTimeLocation = glGetUniformLocation(shader_id, "iTime");
    GLint iResolutionLocation = glGetUniformLocation(shader_id, "iResolution");

    // Set resolution (fixed window size)
    glUniform2f(iResolutionLocation, 800.0f, 600.0f);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Sed
    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
        glUniform1f(iTimeLocation, iTime);

        render_begin();
        {
            // glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        render_end();
    }

    // Free Everything ========================================================
    render_shader_unload(shader_id);
    glDeleteBuffers(1, &vbo);
    os_file_close(frag_file);
    render_deinit();
    os_memory_free(buffer, size);
    wl_window_close();
}
