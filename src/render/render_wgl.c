internal Void_Proc *render_opengl_load_procedure(char *name)
{
    Void_Proc *p = (Void_Proc *)(void *)wglGetProcAddress(name);
    if(p == (Void_Proc*)1 || p == (Void_Proc*)2 || p == (Void_Proc*)3 || p == (Void_Proc*)-1)
    {
        p = 0;
    }
    return p;
}

internal void render_init(void)
{
    // Get device contex
    render_wgl_state.hdc = GetDC(wl_win32_state.handle); 

    // Build pixel format descriptor
    int pf = 0;
    {
        PIXELFORMATDESCRIPTOR pfd = ZERO_STRUCT;
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pf = ChoosePixelFormat(render_wgl_state.hdc, &pfd);
        DescribePixelFormat(render_wgl_state.hdc, pf, sizeof(pfd), &pfd);
        SetPixelFormat(render_wgl_state.hdc, pf, &pfd);
    }
    
    // Make bootstrap contex
    HGLRC bootstrap_ctx = wglCreateContext(render_wgl_state.hdc);
    wglMakeCurrent(render_wgl_state.hdc, bootstrap_ctx);

    // Load extensions
    wglChoosePixelFormatARB    = (FNWGLCHOOSEPIXELFORMATARBPROC*)(void*)render_opengl_load_procedure("wglChoosePixelFormatARB");
    wglCreateContextAttribsARB = (FNWGLCREATECONTEXTATTRIBSARBPROC*)(void*)render_opengl_load_procedure("wglCreateContextAttribsARB");
    wglSwapIntervalEXT         = (FNWGLSWAPINTERVALEXTPROC*)(void*)render_opengl_load_procedure("wglSwapIntervalEXT");

    // Set up real pixel format
    {
        int pf_attribs_i[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, 1,
            WGL_SUPPORT_OPENGL_ARB, 1,
            WGL_DOUBLE_BUFFER_ARB, 1,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0
        };
        UINT num_formats = 0;
        wglChoosePixelFormatARB(render_wgl_state.hdc, pf_attribs_i, 0, 1, &pf, &num_formats);
    }

    // Make real OpenGl contex
    HGLRC real_ctx = 0;
    if(pf)
    {
        int context_attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0
        };
        real_ctx = wglCreateContextAttribsARB(render_wgl_state.hdc, bootstrap_ctx, context_attribs);
        // r_ogl_w32_hglrc = real_ctx;
    }

    //- rjf: clean up bootstrap context
    wglMakeCurrent(render_wgl_state.hdc, 0);
    wglDeleteContext(bootstrap_ctx);
    wglMakeCurrent(render_wgl_state.hdc, real_ctx);
    wglSwapIntervalEXT(1);

#define X(name, r, p) name = (name##_FunctionType *)(void*)render_opengl_load_procedure(#name);
    RenderOpenglXMacro
#undef X
    // glGenTextures(1, &render_wgl_state.tex_handle);
}

internal void render_deinit(void)
{
    ReleaseDC(wl_win32_state.handle, render_wgl_state.hdc);
}

internal void render_begin(void)
{
}

internal void render_end(void)
{
    // glViewport(0, 0, wl_get_window_width(), wl_get_window_height());

    // glBindTexture(GL_TEXTURE_2D, render_wgl_state.tex_handle);
    // glTexImage2D(
    //     GL_TEXTURE_2D, 0, GL_RGBA8, 
    //     wl_get_window_width(), wl_get_display_height(), 
    //     0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, render_wgl_state.memory
    // );
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //
    // glEnable(GL_TEXTURE_2D);
    //
    // glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glMatrixMode(GL_TEXTURE);
    // glLoadIdentity();
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    //
    // glBegin(GL_TRIANGLES);
    // {
    //     F32 p = 1.0f;
    //
    //     // Lower Triangle
    //     glTexCoord2f(0.0f, 0.0f);
    //     glVertex2f(-p, -p);
    //     glTexCoord2f(1.0f, 0.0f);
    //     glVertex2f(p, -p);
    //     glTexCoord2f(1.0f, 1.0f);
    //     glVertex2f(p, p);
    //
    //     // Upper Triangle
    //     glTexCoord2f(0.0f, 0.0f);
    //     glVertex2f(-p, -p);
    //     glTexCoord2f(1.0f, 1.0f);
    //     glVertex2f(p, p);
    //     glTexCoord2f(0.0f, 1.0f);
    //     glVertex2f(-p, p);
    // }
    // glEnd();

    SwapBuffers(render_wgl_state.hdc);
}
