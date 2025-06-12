internal Draw_Buffer
render_init(Alloc alloc)
{

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    EGLint config_attrs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    EGLConfig config;
    EGLint num_configs;
    eglChooseConfig(display, config_attrs, &config, 1, &num_configs);

    EGLSurface surface = eglCreateWindowSurface(display, config, wl_x11_state.window, NULL);

    EGLint ctx_attrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attrs);

    eglMakeCurrent(display, surface, surface, context);

    // Initialize States ======================================================
    render_opengl_state.display = display;
    render_opengl_state.context = context;
    render_opengl_state.surface = surface;

    // Initialize Draw_Buffer =================================================
    I16 width = wl_get_display_width();
    I16 height = wl_get_display_height();
    I32 bytes_per_pixel = 4;
    U64 memory_size = (width * height) * bytes_per_pixel;
    U8 *memory = alloc_make(alloc, U8, memory_size);

    Draw_Buffer draw_buffer = ZERO_STRUCT;
    draw_buffer.memory = memory;
    draw_buffer.width = width;
    draw_buffer.height = height;
    draw_buffer.bytes_per_pixel = bytes_per_pixel;
    draw_buffer.pitch = draw_buffer.width * draw_buffer.bytes_per_pixel;

    return draw_buffer;
}

internal void
render_deinit(void)
{
    eglMakeCurrent(
        render_opengl_state.display,
        EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT
    );
    eglDestroyContext(render_opengl_state.display, render_opengl_state.context);
    eglDestroySurface(render_opengl_state.display, render_opengl_state.surface);
    eglTerminate(render_opengl_state.display);
}

const char* vertexShaderSource = "attribute vec2 a_position;\n"
    "void main() {\n"
        "gl_Position = vec4(a_position, 0.0, 1.0);\n"
    "}";

const char* fragmentShaderSource = "precision mediump float;\n"
    "void main() {\n"
        "gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color\n"
    "}";

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Create and link program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void renderTriangles() {

    F32 p = 0.9f;
    GLfloat vertices[] = {
        // Lower Triangle
        -p, -p,
         p, -p,
         p,  p,
        // Upper Triangle
        -p, -p,
         p,  p,
        -p,  p
    };

    // Create and bind VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create shader program
    GLuint program = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(program);

    // Set up vertex attribute
    GLint positionLoc = glGetAttribLocation(program, "a_position");
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Draw triangles
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
}

internal void
render_begin(void)
{
}

internal void
render_end(void)
{
}
