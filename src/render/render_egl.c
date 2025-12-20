// OpenGL helper functions
// ============================================================================

internal Void_Proc *_render_opengl_load_procedure(char *name)
{
    Void_Proc *p = (Void_Proc *)(void *)eglGetProcAddress(name);
    if(p == (Void_Proc*)1 || p == (Void_Proc*)2 || p == (Void_Proc*)3 || p == (Void_Proc*)-1)
    {
        p = 0;
    }
    return p;
}

// Internal OpenGL functions
// ============================================================================

internal void _render_opengl_init(void)
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
        display, config, (EGLNativeWindowType)_wl_x11_state.window, NULL
    );

    EGLint ctx_attrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attrs);

    eglMakeCurrent(display, surface, surface, context);

    // Initialize States ======================================================
    _render_egl_state.display = display;
    _render_egl_state.context = context;
    _render_egl_state.surface = surface;
}

internal void _render_opengl_deinit(void)
{
    eglMakeCurrent(
        _render_egl_state.display,
        EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT
    );
    eglDestroyContext(_render_egl_state.display, _render_egl_state.context);
    eglDestroySurface(_render_egl_state.display, _render_egl_state.surface);
    eglTerminate(_render_egl_state.display);
}

internal void _render_opengl_begin(void)
{
}

internal void _render_opengl_end(void)
{
    eglSwapBuffers(_render_egl_state.display, _render_egl_state.surface);
}
