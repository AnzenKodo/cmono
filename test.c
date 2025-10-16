#include <windows.h>
#include <glad/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Create Win32 window
HWND CreateWin32Window(HINSTANCE hInstance, int width, int height) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "EGLWindow"; // Use narrow-character string
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("EGLWindow", "Glad EGL Red Screen", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, SW_SHOW);
    return hwnd;
}

int main() {
    // Initialize Win32 window
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hwnd = CreateWin32Window(hInstance, 800, 600);
    if (!hwnd) {
        printf("Failed to create window\n");
        return -1;
    }

    HDC hdc = GetDC(hwnd);

    // Initialize EGL display
    EGLDisplay display = eglGetDisplay((EGLNativeDisplayType)hdc);
    if (display == EGL_NO_DISPLAY) {
        printf("Failed to get EGL display\n");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    // Initialize Glad EGL with display and loader
    if (!gladLoadEGL(display, eglGetProcAddress)) {
        printf("Failed to load EGL\n");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        printf("Failed to initialize EGL display\n");
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }
    printf("Loaded EGL %d.%d\n", major, minor);

    // Configure EGL
    EGLint configAttributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttributes, &config, 1, &numConfigs) || numConfigs == 0) {
        printf("Failed to choose EGL config\n");
        eglTerminate(display);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    // Create EGL surface
    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hwnd, NULL);
    if (surface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface\n");
        eglTerminate(display);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    // Create EGL context
    EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
    if (context == EGL_NO_CONTEXT) {
        printf("Failed to create EGL context\n");
        eglDestroySurface(display, surface);
        eglTerminate(display);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    // Make the context current
    if (!eglMakeCurrent(display, surface, surface, context)) {
        printf("Failed to make EGL context current\n");
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return -1;
    }

    // Main loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Clear screen to red
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        eglSwapBuffers(display, surface);
    }

    // Cleanup
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);

    return 0;
}
