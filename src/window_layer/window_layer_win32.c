// Basic Window functions
//=============================================================================

LRESULT CALLBACK wl_w32_window_proc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
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

        case WM_NCACTIVATE: {
        }break;
        case WM_NCCALCSIZE: {
        }break;
        case WM_NCHITTEST: {
                // if(window == 0 || window->custom_border == 0 || is_fullscreen)
                // {
                //     result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
                // }
                // else
                // {
                //     result = DefWindowProc(hwnd, uMsg, wParam, lParam);
                //     switch (result)
                //     {
                //         case HTNOWHERE:
                //         case HTRIGHT:
                //         case HTLEFT:
                //         case HTTOPLEFT:
                //         case HTTOPRIGHT:
                //         case HTBOTTOMRIGHT:
                //         case HTBOTTOM:
                //         case HTBOTTOMLEFT: {
                //         } break;
                //     }
        }break;
    }
    return result;
}
#include <stdio.h>
internal void wl_window_open(Str8 title, Vec2I32 win_size)
{
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASSEXW wc = ZERO_STRUCT;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = wl_w32_window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = L"SimpleWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, MAKEINTRESOURCE(1));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(
            NULL, L"Failed to register window class!", L"Error",
            MB_OK | MB_ICONERROR
        );
        os_exit(1);
    }

    Str16 title16 = str16_from_8(os_core_state.alloc, title);
    // HWND hwnd = CreateWindowExW(
    //     WS_EX_APPWINDOW,
    //     TEXT("graphical-window"), L"Hello", //title16.str,
    //     WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
    //     CW_USEDEFAULT, CW_USEDEFAULT,
    //     win_size.x, win_size.y,
    //     0, 0,
    //     instance, 0
    // );
    // ShowWindow(hwnd, SW_SHOW);
    // UpdateWindow(hwnd);

    // MSG msg;
    // while (GetMessage(&msg, NULL, 0, 0)) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }
}

internal void wl_window_icon_set(U32 *icon_data, U32 width, U32 height)
{
}

internal void wl_window_close(void)
{
}

// Event Functions
//=============================================================================

internal Wl_Event wl_get_event(void)
{
    Wl_Event event = ZERO_STRUCT;
    return event;
}
