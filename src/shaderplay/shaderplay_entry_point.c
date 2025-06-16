#define RENDER_BACKEND RENDER_BACKEND_OPENGL

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

// internal Str8
// os_file_read_str(Arena *arena, Os_File file, Rng1U64 range)
// {
//     Str8 result;
//     // result.size = dim_1u64(range);
//     // result.str = push_array_no_zero(arena, U8, result.size);
//     // U64 actual_read_size = os_file_read(file, range, result.str);
//     // if(actual_read_size < result.size)
//     // {
//     //     arena_pop_to(arena, pre_pos + actual_read_size);
//     //     result.size = actual_read_size;
//     // }
//     return result;
// }

internal void
entry_point(char *argv[])
{
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    U8 *memory = alloc_make(alloc, U8, 1);

    Os_File file = os_file_open(str8("build.c"), OS_AccessFlag_Write);
    Os_FileProperties prop = os_file_properties(file);
    os_file_read(file, rng_1u64(1, prop.size), &memory);
    printf("%ld: %s\n", prop.size, memory);
    os_file_close(file);
}

char* vertexShaderSource =
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec4 position;\n"
    "void main() {\n"
    "    gl_Position = position;\n"
    "}\n";

char* fragmentShaderSource =
    "#version 300 es\n"
    "precision highp float;\n"
    "uniform vec2 iResolution;\n"
    "uniform float iTime;\n"
    "out vec4 fragColor;\n"
    "\n"
    "vec2 march(vec3 pos, vec3 dir);\n"
    "vec3 camera(vec2 uv);\n"
    "void rotate(inout vec2 v, float angle);\n"
    "float mylength(vec2 p);\n"
    "\n"
    "#define LIGHT\n"
    "//#define HOLES\n"
    "//#define FUNKY\n"
    "\n"
    "float t;\n"
    "vec3 ret_col;\n"
    "#ifdef LIGHT\n"
    "vec3 h;\n"
    "#endif\n"
    "\n"
    "#define I_MAX 200.0\n"
    "#define E 0.0001\n"
    "#define FAR 50.0\n"
    "#define PI 3.14159\n"
    "#define TAU PI*2.0\n"
    "\n"
    "vec2 modA(vec2 p, float count) {\n"
    "    float an = TAU/count;\n"
    "    float a = atan(p.y,p.x)+an*0.5;\n"
    "    a = mod(a, an)-an*0.5;\n"
    "    return vec2(cos(a),sin(a))*length(p);\n"
    "}\n"
    "\n"
    "float scene(vec3 p) {\n"
    "    float var;\n"
    "    float mind = 1e5;\n"
    "    vec3 op = p;\n"
    "    #ifdef FUNKY\n"
    "    var = step(-1.+cos(floor(p.z*6.)/6.+iTime*1.)*3.14, mod(atan(p.x, p.y), 6.28)-3.14) *\n"
    "          step(mod(atan(p.x, p.y), 6.28)-3.14-1.5, -1.+cos(floor(p.z*3.)/1.+iTime*1.)*3.14) *\n"
    "          step(0.0, (length(fract(vec2(op.z, min(abs(op.x), abs(op.y)))*10.)-0.5)-0.2));\n"
    "    #else\n"
    "    var = atan(p.x, p.y)*1.0+0.0;\n"
    "    #endif\n"
    "    var = cos(var*1.0+floor(p.z)+iTime*(mod(floor(p.z), 2.0)-1.0 == 0.0 ? -1.0 : 1.0));\n"
    "    float dist_cylinder = 1e5;\n"
    "    ret_col = 1.0-vec3(0.5-var*0.5, 0.5, 0.3+var*0.5);\n"
    "    mind = length(p.xy)-1.0+0.1*var;\n"
    "    #ifdef HOLES\n"
    "    mind = max(mind, var*-(length(fract(vec2(op.z, min(abs(op.x), abs(op.y)))*10.0)-0.5)-0.1));\n"
    "    #endif\n"
    "    mind = max(mind, -(length(p.xy)-0.9+0.1*var));\n"
    "    p.xy = modA(p.yx, 50.0+50.0*sin(p.z*0.25));\n"
    "    p.z = fract(p.z*3.0)-0.5;\n"
    "    if (var != 0.0) {\n"
    "        dist_cylinder = length(p.zy)-0.0251-0.25*sin(op.z*5.5);\n"
    "        dist_cylinder = max(dist_cylinder, -p.x+0.4+clamp(var, 0.0, 1.0));\n"
    "    }\n"
    "    mind = min(mind, dist_cylinder);\n"
    "    #ifdef LIGHT\n"
    "    h += vec3(0.5,0.8,0.5)*(var!=0.0?1.0:0.0)*0.0125/(0.01+max(mind-var*0.1, 0.0001)*max(mind-var*0.1, 0.0001));\n"
    "    #endif\n"
    "    return mind;\n"
    "}\n"
    "\n"
    "vec2 march(vec3 pos, vec3 dir) {\n"
    "    vec2 dist = vec2(0.0, 0.0);\n"
    "    vec3 p = vec3(0.0, 0.0, 0.0);\n"
    "    vec2 s = vec2(0.0, 0.0);\n"
    "    for (float i = -1.0; i < I_MAX; ++i) {\n"
    "        p = pos + dir * dist.y;\n"
    "        dist.x = scene(p);\n"
    "        dist.y += dist.x*0.2;\n"
    "        if (dist.x < E || dist.y > FAR) {\n"
    "            break;\n"
    "        }\n"
    "        s.x++;\n"
    "    }\n"
    "    s.y = dist.y;\n"
    "    return s;\n"
    "}\n"
    "\n"
    "float mylength(vec2 p) {\n"
    "    p = p*p*p*p;\n"
    "    p = p*p;\n"
    "    float ret = (p.x+p.y);\n"
    "    ret = pow(ret, 1.0/8.0);\n"
    "    return ret;\n"
    "}\n"
    "\n"
    "void rotate(inout vec2 v, float angle) {\n"
    "    v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);\n"
    "}\n"
    "\n"
    "vec2 rot(vec2 p, vec2 ang) {\n"
    "    float c = cos(ang.x);\n"
    "    float s = sin(ang.y);\n"
    "    mat2 m = mat2(c, -s, s, c);\n"
    "    return (p * m);\n"
    "}\n"
    "\n"
    "vec3 camera(vec2 uv) {\n"
    "    float fov = 1.0;\n"
    "    vec3 forw = vec3(0.0, 0.0, -1.0);\n"
    "    vec3 right = vec3(1.0, 0.0, 0.0);\n"
    "    vec3 up = vec3(0.0, 1.0, 0.0);\n"
    "    return normalize((uv.x) * right + (uv.y) * up + fov * forw);\n"
    "}\n"
    "\n"
    "void mainImage(out vec4 c_out, in vec2 f) {\n"
    "    t = iTime*0.125;\n"
    "    vec3 col = vec3(0.0, 0.0, 0.0);\n"
    "    vec2 R = iResolution.xy;\n"
    "    vec2 uv = vec2(f-R/2.0) / R.y;\n"
    "    vec3 dir = camera(uv);\n"
    "    vec3 pos = vec3(0.0, 0.0, 0.0);\n"
    "    pos.z = 4.5-iTime*2.0;\n"
    "    #ifdef LIGHT\n"
    "    h = vec3(0.0);\n"
    "    #endif\n"
    "    vec2 inter = march(pos, dir);\n"
    "    if (inter.y <= FAR)\n"
    "        col.xyz = ret_col*(1.0-inter.x*0.0025);\n"
    "    else\n"
    "        col *= 0.0;\n"
    "    #ifdef LIGHT\n"
    "    col += h*0.005125;\n"
    "    #endif\n"
    "    c_out = vec4(col, 1.0);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    mainImage(fragColor, gl_FragCoord.xy);\n"
    "}\n";

internal void
entry_point2(char *argv[])
{
    // Program Init ==========================================================
    wl_window_open(str8("Scuttle"), vec2i32(750, 750));
    // wl_window_icon_set(cast(U32 *)ICON, ICON_WIDTH, ICON_HEIGHT);
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    Draw_Buffer draw_buffer = render_init(alloc);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        printf("Shader program linking failed: %s", log);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        printf("Shader program linking failed: %s", log);
        return;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        printf("Shader program linking failed: %s", log);
        return;
    }
    glUseProgram(program);

    // Get uniform locations
    GLint iTimeLocation = glGetUniformLocation(program, "iTime");
    GLint iResolutionLocation = glGetUniformLocation(program, "iResolution");

    // Set resolution (fixed window size)
    glUniform2f(iResolutionLocation, 800.0f, 600.0f);

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

    GLint positionAttrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    U64 start = os_now_microsec();
    while (!wl_should_window_close())
    {
        wl_set_fps(60);
        wl_update_events();
        if (
            wl_is_key_pressed(Wl_Key_Esc) ||
            wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }

        U64 now = os_now_microsec();
        float iTime = cast(float)(now - start) / Million(1);
        glUniform1f(iTimeLocation, iTime);

        render_begin();
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        render_end();
    }

    // Free Everything ========================================================

    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteBuffers(1, &vbo);
    render_deinit();
    os_memory_free(buffer, size);
    wl_window_close();
}
