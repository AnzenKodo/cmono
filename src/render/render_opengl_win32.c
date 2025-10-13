internal Draw_Buffer render_init(Alloc alloc)
{
    render_opengl_win32_state.hdc = GetDC(wl_win32_state.handle); 

    PIXELFORMATDESCRIPTOR disired_pixel_format = ZERO_STRUCT;
    disired_pixel_format.nSize      = sizeof(disired_pixel_format);
    disired_pixel_format.nVersion   = 1;
    disired_pixel_format.dwFlags    = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    disired_pixel_format.cColorBits = 32;
    disired_pixel_format.cAlphaBits = 8;
    disired_pixel_format.iLayerType = PFD_MAIN_PLANE;

    int suggest_pixel_format_index = ChoosePixelFormat(
        render_opengl_win32_state.hdc, &disired_pixel_format
    );
    PIXELFORMATDESCRIPTOR suggest_pixel_format;
    DescribePixelFormat(
        render_opengl_win32_state.hdc, suggest_pixel_format_index,
        sizeof(suggest_pixel_format), &suggest_pixel_format
    );
    SetPixelFormat(
        render_opengl_win32_state.hdc, suggest_pixel_format_index, &suggest_pixel_format
    );

    HGLRC opengl_rc = wglCreateContext(render_opengl_win32_state.hdc);
    if (wglMakeCurrent(render_opengl_win32_state.hdc, opengl_rc))
    {
    }
    else
    {
        InvalidCodePath;
    }

    glGenTextures(1, &render_opengl_win32_state.tex_handle);

    // Initialize States ======================================================
    Draw_Buffer draw_buffer     = ZERO_STRUCT;
    draw_buffer.width           = wl_get_display_width();
    draw_buffer.height          = wl_get_display_height();
    draw_buffer.bytes_per_pixel = 4;
    draw_buffer.pitch           = draw_buffer.width * draw_buffer.bytes_per_pixel;
    U64 memory_size             = (draw_buffer.width * draw_buffer.height) * draw_buffer.bytes_per_pixel;
    draw_buffer.memory          = alloc_make(alloc, U8, memory_size);

    render_opengl_win32_state.memory = draw_buffer.memory;
    
    return draw_buffer;
}

internal void render_deinit(void)
{
    ReleaseDC(wl_win32_state.handle, render_opengl_win32_state.hdc);
}

internal void render_begin(void)
{
}

internal void render_end(void)
{
    glViewport(0, 0, wl_get_window_width(), wl_get_window_height());

    glBindTexture(GL_TEXTURE_2D, render_opengl_win32_state.tex_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, wl_get_window_width(), wl_get_display_height(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, render_opengl_win32_state.memory);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable(GL_TEXTURE_2D);

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
    {
        F32 p = 1.0f;

        // Lower Triangle
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-p, -p);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(p, -p);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(p, p);

        // Upper Triangle
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-p, -p);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(p, p);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-p, p);
    }
    glEnd();

    SwapBuffers(render_opengl_win32_state.hdc);
}
