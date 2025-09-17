// Basic Window functions
//=============================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);  // Quit the message loop
            return 0;
LRESULT CALLBACK
wl_w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch(uMsg)
    {
        default: {
            result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }break;

        case WM_ENTERSIZEMOVE: {
        }break;
        case WM_EXITSIZEMOVE: {
        }break;

        case WM_SIZE:
        case WM_PAINT: {
        }break;

        case WM_CLOSE: {
        }break;

        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
        } // fallthrough;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: {
            switch (uMsg)
            {
                case WM_LBUTTONUP: case WM_LBUTTONDOWN: {
                }break;
                case WM_MBUTTONUP: case WM_MBUTTONDOWN: {
                }break;
                case WM_RBUTTONUP: case WM_RBUTTONDOWN: {
                }break;
            }
        }break;
        case WM_MOUSEMOVE: {
        }break;
        case WM_MOUSEWHEEL: {
        }break;

        case WM_MOUSEHWHEEL: {
        }break;

        case WM_SYSKEYDOWN: case WM_SYSKEYUP: {
        } // fallthrough;
        case WM_KEYDOWN: case WM_KEYUP: {
        }break;

        case WM_SYSCHAR: {
        }break;

        case WM_CHAR: {
        }break;

        case WM_KILLFOCUS: {
        }break;

        case WM_SETCURSOR: {
        }break;

        case WM_DPICHANGED: {
        }break;

            //- rjf: [file drop]
        case WM_DROPFILES: {
        }break;

            //- rjf: [custom border]
        case WM_NCPAINT: {
        }break;
        case WM_DWMCOMPOSITIONCHANGED: {
        }break;
        case WM_WINDOWPOSCHANGED: {
        }break;
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME: {
        }break;
        case WM_SETICON:
        case WM_SETTEXT: {
        }break;

        case WM_NCACTIVATE: {
        }break;
        case WM_NCCALCSIZE: {
        }break;
        case WM_NCHITTEST: {
                if(window == 0 || window->custom_border == 0 || is_fullscreen)
                {
                    result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
                }
                else
                {
                    result = DefWindowProc(hwnd, uMsg, wParam, lParam);
                    switch (result)
                    {
                        case HTNOWHERE:
                        case HTRIGHT:
                        case HTLEFT:
                        case HTTOPLEFT:
                        case HTTOPRIGHT:
                        case HTBOTTOMRIGHT:
                        case HTBOTTOM:
                        case HTBOTTOMLEFT: {
                        } break;
                    }
        }break;
    }
    ProfEnd();
    return result;
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
