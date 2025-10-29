internal void render_init(void)
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

    EGLSurface surface = eglCreateWindowSurface(
        display, config, Cast(EGLNativeWindowType)wl_x11_state.window, NULL
    );

    EGLint ctx_attrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attrs);

    eglMakeCurrent(display, surface, surface, context);

    // Initialize States ======================================================
    render_egl_state.display = display;
    render_egl_state.context = context;
    render_egl_state.surface = surface;
}

internal void render_deinit(void)
{
    eglMakeCurrent(
        render_egl_state.display,
        EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT
    );
    eglDestroyContext(render_egl_state.display, render_egl_state.context);
    eglDestroySurface(render_egl_state.display, render_egl_state.surface);
    eglTerminate(render_egl_state.display);
}

internal void render_begin(void)
{
}

internal void render_end(void)
{
    eglSwapBuffers(render_egl_state.display, render_egl_state.surface);
}
