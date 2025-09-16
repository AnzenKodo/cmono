// Basic Window functions
//=============================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);  // Quit the message loop
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect;
            GetClientRect(hwnd, &rect);
            DrawText(hdc, TEXT("Hello, Win32!"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

internal void
wl_window_open(Str8 title, Vec2I32 win_size)
{
    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASSEXW wc = {
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .hInstance = instance,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszClassName = TEXT("SimpleWindowClass")
    };

    RegisterClassExW(&wc);

    // Create the window
    HWND hwnd = CreateWindow(
        TEXT("SimpleWindowClass"),
        TEXT("Simple Win32 Window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, instance, NULL
    );

    // Show the window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

internal void wl_window_icon_set(U32 *icon_data, U32 width, U32 height)
{
}

internal void
wl_window_close(void)
{
}

// Event Functions
//=============================================================================

internal Wl_Event wl_get_event(void)
{
}
