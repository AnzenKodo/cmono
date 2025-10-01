// Win32 Helper Functions
//=============================================================================

internal Wl_Key os_w32_os_key_from_vkey(WPARAM vkey)
{
    local_persist I32 first = 1;
    local_persist Wl_Key key_table[256];
    if (first)
    {
        first = 0;
        MemoryZeroArray(key_table);

        key_table[(unsigned int)'A'] = Wl_Key_A;
        key_table[(unsigned int)'B'] = Wl_Key_B;
        key_table[(unsigned int)'C'] = Wl_Key_C;
        key_table[(unsigned int)'D'] = Wl_Key_D;
        key_table[(unsigned int)'E'] = Wl_Key_E;
        key_table[(unsigned int)'F'] = Wl_Key_F;
        key_table[(unsigned int)'G'] = Wl_Key_G;
        key_table[(unsigned int)'H'] = Wl_Key_H;
        key_table[(unsigned int)'I'] = Wl_Key_I;
        key_table[(unsigned int)'J'] = Wl_Key_J;
        key_table[(unsigned int)'K'] = Wl_Key_K;
        key_table[(unsigned int)'L'] = Wl_Key_L;
        key_table[(unsigned int)'M'] = Wl_Key_M;
        key_table[(unsigned int)'N'] = Wl_Key_N;
        key_table[(unsigned int)'O'] = Wl_Key_O;
        key_table[(unsigned int)'P'] = Wl_Key_P;
        key_table[(unsigned int)'Q'] = Wl_Key_Q;
        key_table[(unsigned int)'R'] = Wl_Key_R;
        key_table[(unsigned int)'S'] = Wl_Key_S;
        key_table[(unsigned int)'T'] = Wl_Key_T;
        key_table[(unsigned int)'U'] = Wl_Key_U;
        key_table[(unsigned int)'V'] = Wl_Key_V;
        key_table[(unsigned int)'W'] = Wl_Key_W;
        key_table[(unsigned int)'X'] = Wl_Key_X;
        key_table[(unsigned int)'Y'] = Wl_Key_Y;
        key_table[(unsigned int)'Z'] = Wl_Key_Z;

        for (U64 i = '0', j = Wl_Key_0; i <= '9'; i += 1, j += 1)
        {
            key_table[i] = (Wl_Key)j;
        }
        for (U64 i = VK_NUMPAD0, j = Wl_Key_0; i <= VK_NUMPAD9; i += 1, j += 1)
        {
            key_table[i] = (Wl_Key)j;
        }
        for (U64 i = VK_F1, j = Wl_Key_F1; i <= VK_F24; i += 1, j += 1)
        {
            key_table[i] = (Wl_Key)j;
        }

        key_table[VK_SPACE]     = Wl_Key_Space;
        key_table[VK_OEM_3]     = Wl_Key_Tick;
        key_table[VK_OEM_MINUS] = Wl_Key_Minus;
        key_table[VK_OEM_PLUS]  = Wl_Key_Equal;
        key_table[VK_OEM_4]     = Wl_Key_LeftBracket;
        key_table[VK_OEM_6]     = Wl_Key_RightBracket;
        key_table[VK_OEM_1]     = Wl_Key_Semicolon;
        key_table[VK_OEM_7]     = Wl_Key_Quote;
        key_table[VK_OEM_COMMA] = Wl_Key_Comma;
        key_table[VK_OEM_PERIOD]= Wl_Key_Period;
        key_table[VK_OEM_2]     = Wl_Key_Slash;
        key_table[VK_OEM_5]     = Wl_Key_BackSlash;

        key_table[VK_TAB]       = Wl_Key_Tab;
        key_table[VK_PAUSE]     = Wl_Key_Pause;
        key_table[VK_ESCAPE]    = Wl_Key_Esc;

        key_table[VK_UP]        = Wl_Key_Up;
        key_table[VK_LEFT]      = Wl_Key_Left;
        key_table[VK_DOWN]      = Wl_Key_Down;
        key_table[VK_RIGHT]     = Wl_Key_Right;

        key_table[VK_BACK]      = Wl_Key_Backspace;
        key_table[VK_RETURN]    = Wl_Key_Return;

        key_table[VK_DELETE]    = Wl_Key_Delete;
        key_table[VK_INSERT]    = Wl_Key_Insert;
        key_table[VK_PRIOR]     = Wl_Key_PageUp;
        key_table[VK_NEXT]      = Wl_Key_PageDown;
        key_table[VK_HOME]      = Wl_Key_Home;
        key_table[VK_END]       = Wl_Key_End;

        key_table[VK_CAPITAL]   = Wl_Key_CapsLock;
        key_table[VK_NUMLOCK]   = Wl_Key_NumLock;
        key_table[VK_SCROLL]    = Wl_Key_ScrollLock;
        key_table[VK_APPS]      = Wl_Key_Menu;

        key_table[VK_CONTROL]   = Wl_Key_Ctrl;
        key_table[VK_LCONTROL]  = Wl_Key_CtrlLeft;
        key_table[VK_RCONTROL]  = Wl_Key_CtrlRight;
        key_table[VK_SHIFT]     = Wl_Key_Shift;
        key_table[VK_LSHIFT]    = Wl_Key_ShiftLeft;
        key_table[VK_RSHIFT]    = Wl_Key_ShiftRight;
        key_table[VK_MENU]      = Wl_Key_Alt;
        key_table[VK_LMENU]     = Wl_Key_AltLeft;
        key_table[VK_RMENU]     = Wl_Key_AltRight;

        key_table[VK_DIVIDE]   = Wl_Key_NumSlash;
        key_table[VK_MULTIPLY] = Wl_Key_NumStar;
        key_table[VK_SUBTRACT] = Wl_Key_NumMinus;
        key_table[VK_ADD]      = Wl_Key_NumPlus;
        key_table[VK_DECIMAL]  = Wl_Key_NumPeriod;

        for (U32 i = 0; i < 10; i += 1)
        {
            key_table[VK_NUMPAD0 + i] = (Wl_Key)((U64)Wl_Key_Num0 + i);
        }

        for (U64 i = 0xDF, j = 0; i < 0xFF; i += 1, j += 1)
        {
            key_table[i] = (Wl_Key)((U64)Wl_Key_Ex0 + j);
        }
    }

    Wl_Key key = key_table[vkey&bitmask8];
    return key;
}

internal WPARAM os_w32_vkey_from_os_key(Wl_Key key)
{
    WPARAM result = 0;
    {
        local_persist I32 initialized = 0;
        local_persist WPARAM vkey_table[Wl_Key_COUNT] = {0};
        if(initialized == 0)
        {
            initialized = 1;
            vkey_table[Wl_Key_Esc] = VK_ESCAPE;
            for(Wl_Key key = Wl_Key_F1; key <= Wl_Key_F24; key = (Wl_Key)(key+1))
            {
                vkey_table[key] = VK_F1+(key-Wl_Key_F1);
            }
            vkey_table[Wl_Key_Tick] = VK_OEM_3;
            for(Wl_Key key = Wl_Key_0; key <= Wl_Key_9; key = (Wl_Key)(key+1))
            {
                vkey_table[key] = '0'+(key-Wl_Key_0);
            }
            vkey_table[Wl_Key_Minus]     = VK_OEM_MINUS;
            vkey_table[Wl_Key_Equal]     = VK_OEM_PLUS;
            vkey_table[Wl_Key_Backspace] = VK_BACK;
            vkey_table[Wl_Key_Tab] = VK_TAB;
            vkey_table[Wl_Key_Q] = 'Q';
            vkey_table[Wl_Key_W] = 'W';
            vkey_table[Wl_Key_E] = 'E';
            vkey_table[Wl_Key_R] = 'R';
            vkey_table[Wl_Key_T] = 'T';
            vkey_table[Wl_Key_Y] = 'Y';
            vkey_table[Wl_Key_U] = 'U';
            vkey_table[Wl_Key_I] = 'I';
            vkey_table[Wl_Key_O] = 'O';
            vkey_table[Wl_Key_P] = 'P';
            vkey_table[Wl_Key_LeftBracket]  = VK_OEM_4;
            vkey_table[Wl_Key_RightBracket] = VK_OEM_6;
            vkey_table[Wl_Key_BackSlash]    = VK_OEM_5;
            vkey_table[Wl_Key_CapsLock]     = VK_CAPITAL;
            vkey_table[Wl_Key_A] = 'A';
            vkey_table[Wl_Key_S] = 'S';
            vkey_table[Wl_Key_D] = 'D';
            vkey_table[Wl_Key_F] = 'F';
            vkey_table[Wl_Key_G] = 'G';
            vkey_table[Wl_Key_H] = 'H';
            vkey_table[Wl_Key_J] = 'J';
            vkey_table[Wl_Key_K] = 'K';
            vkey_table[Wl_Key_L] = 'L';
            vkey_table[Wl_Key_Semicolon] = VK_OEM_1;
            vkey_table[Wl_Key_Quote]     = VK_OEM_7;
            vkey_table[Wl_Key_Return]    = VK_RETURN;
            vkey_table[Wl_Key_Shift]     = VK_SHIFT;
            vkey_table[Wl_Key_Z] = 'Z';
            vkey_table[Wl_Key_X] = 'X';
            vkey_table[Wl_Key_C] = 'C';
            vkey_table[Wl_Key_V] = 'V';
            vkey_table[Wl_Key_B] = 'B';
            vkey_table[Wl_Key_N] = 'N';
            vkey_table[Wl_Key_M] = 'M';
            vkey_table[Wl_Key_Comma]      = VK_OEM_COMMA;
            vkey_table[Wl_Key_Period]     = VK_OEM_PERIOD;
            vkey_table[Wl_Key_Slash]      = VK_OEM_2;
            vkey_table[Wl_Key_Ctrl]       = VK_CONTROL;
            vkey_table[Wl_Key_Alt]        = VK_MENU;
            vkey_table[Wl_Key_Space]      = VK_SPACE;
            vkey_table[Wl_Key_Menu]       = VK_APPS;
            vkey_table[Wl_Key_ScrollLock] = VK_SCROLL;
            vkey_table[Wl_Key_Pause]      = VK_PAUSE;
            vkey_table[Wl_Key_Insert]     = VK_INSERT;
            vkey_table[Wl_Key_Home]       = VK_HOME;
            vkey_table[Wl_Key_PageUp]     = VK_PRIOR;
            vkey_table[Wl_Key_Delete]     = VK_DELETE;
            vkey_table[Wl_Key_End]        = VK_END;
            vkey_table[Wl_Key_PageDown]   = VK_NEXT;
            vkey_table[Wl_Key_Up]         = VK_UP;
            vkey_table[Wl_Key_Left]       = VK_LEFT;
            vkey_table[Wl_Key_Down]       = VK_DOWN;
            vkey_table[Wl_Key_Right]      = VK_RIGHT;
            for(Wl_Key key = Wl_Key_Ex0; key <= Wl_Key_Ex29; key = (Wl_Key)(key+1))
            {
                vkey_table[key] = 0xDF + (key-Wl_Key_Ex0);
            }
            vkey_table[Wl_Key_NumLock]   = VK_NUMLOCK;
            vkey_table[Wl_Key_NumSlash]  = VK_DIVIDE;
            vkey_table[Wl_Key_NumStar]   = VK_MULTIPLY;
            vkey_table[Wl_Key_NumMinus]  = VK_SUBTRACT;
            vkey_table[Wl_Key_NumPlus]   = VK_ADD;
            vkey_table[Wl_Key_NumPeriod] = VK_DECIMAL;
            vkey_table[Wl_Key_Num0] = VK_NUMPAD0;
            vkey_table[Wl_Key_Num1] = VK_NUMPAD1;
            vkey_table[Wl_Key_Num2] = VK_NUMPAD2;
            vkey_table[Wl_Key_Num3] = VK_NUMPAD3;
            vkey_table[Wl_Key_Num4] = VK_NUMPAD4;
            vkey_table[Wl_Key_Num5] = VK_NUMPAD5;
            vkey_table[Wl_Key_Num6] = VK_NUMPAD6;
            vkey_table[Wl_Key_Num7] = VK_NUMPAD7;
            vkey_table[Wl_Key_Num8] = VK_NUMPAD8;
            vkey_table[Wl_Key_Num9] = VK_NUMPAD9;
            vkey_table[Wl_Key_LeftMouseButton]   = VK_LBUTTON;
            vkey_table[Wl_Key_MiddleMouseButton] = VK_MBUTTON;
            vkey_table[Wl_Key_RightMouseButton]  = VK_RBUTTON;
        }
        result = vkey_table[key];
    }
    return result;
}

LRESULT CALLBACK wl_w32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Wl_Event event = ZERO_STRUCT;
    LRESULT result = 0;
    bool release = 0;

    switch(uMsg)
    {
        default:
        {
            result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }break;

        case WM_ENTERSIZEMOVE:
        {
            event.type = Wl_EventType_WindowResize;
        } break;
        case WM_EXITSIZEMOVE:
        {
        }break;

        case WM_SIZE:
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect = {20, 20, 200, 100};
            HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0)); 
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);
            EndPaint(hwnd, &ps);  // End painting [2]
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }break;

        case WM_CLOSE:
        {
            event.type = Wl_EventType_WindowClose;
        }break;

        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            release = 1;
        } // fallthrough;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            // if (release) 
            // {
            //     event.type = Wl_EventType_Release;
            // } 
            // else 
            // {
            //     event.type = Wl_EventType_Press;
            // }
            // switch (uMsg)
            // {
            //     case WM_LBUTTONUP: case WM_LBUTTONDOWN:
            //     {
            //         event.key = Wl_Key_LeftMouseButton;
            //     }break;
            //     case WM_MBUTTONUP: case WM_MBUTTONDOWN:
            //     {
            //         event.key = Wl_Key_MiddleMouseButton;
            //     }break;
            //     case WM_RBUTTONUP: case WM_RBUTTONDOWN:
            //     {
            //         event.key = Wl_Key_RightMouseButton;
            //     }break;
            // }
            // event.pos.x = (F32)(I16)LOWORD(lParam);
            // event.pos.y = (F32)(I16)HIWORD(lParam);
            // if(release) 
            // {
            //     ReleaseCapture();
            // } 
            // else 
            // {
            //     SetCapture(hwnd);
            // }
        }break;
        case WM_MOUSEMOVE:
        {
            event.pos.x = (F32)(I16)LOWORD(lParam);
            event.pos.y = (F32)(I16)HIWORD(lParam);
        }break;

        case WM_MOUSEWHEEL:
        {
            // I16 wheel_delta = HIWORD(wParam);
            // Wl_Event *event = os_w32_push_event(Wl_EventKind_Scroll, window);
            // POINT p;
            // p.x = (I32)(I16)LOWORD(lParam);
            // p.y = (I32)(I16)HIWORD(lParam);
            // ScreenToClient(window->hwnd, &p);
            // event->pos.x = (F32)p.x;
            // event->pos.y = (F32)p.y;
            // event->delta = vec_2f32(0.f, -(F32)wheel_delta);
        }break;
        case WM_MOUSEHWHEEL:
        {
            I16 wheel_delta = HIWORD(wParam);
            POINT p;
            p.x = (I32)LOWORD(lParam);
            p.y = (I32)HIWORD(lParam);
            ScreenToClient(hwnd, &p);
            event.pos.x = (F32)p.x;
            event.pos.y = (F32)p.y;
            event.delta = vec_2f32((F32)wheel_delta, 0.f);
        }break;

        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
        {
            if(
                wParam != VK_MENU &&
                (wParam < VK_F1 || VK_F24 < wParam || wParam == VK_F4)
            ) {
              result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
            }
        } // fallthrough;
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
        //     I32 was_down = (lParam & bit31);
        //     I32 is_down  = !(lParam & bit32);
        //
        //     I32 is_repeat = 0;
        //     if(!is_down) {
        //         release = 1;
        //     } else if(was_down) {
        //         is_repeat = 1;
        //     }
        //
        //     I32 right_sided = 0;
        //     if ((lParam & bit25) &&
        //         (wParam == VK_CONTROL || wParam == VK_RCONTROL ||
        //          wParam == VK_MENU || wParam == VK_RMENU ||
        //          wParam == VK_SHIFT || wParam == VK_RSHIFT))
        //     {
        //         right_sided = 1;
        //     }
        //
        //     if (release)
        //     {
        //         event.type = Wl_EventKind_Release;
        //         event.type = Wl_EventKind_Press;
        //     }
        //     event.key = os_w32_os_key_from_vkey(wParam);
        //     event.repeat_count = lParam & bitmask16;
        //     event.is_repeat = is_repeat;
        //     event.right_sided = right_sided;
        //     if (event->key == Wl_Key_Alt   && event->modifiers & Wl_Modifier_Alt)
        //     {
        //         event->modifiers &= ~Wl_Modifier_Alt;
        //     }
        //     if(event->key == Wl_Key_Ctrl  && event->modifiers & Wl_Modifier_Ctrl)
        //     {
        //         event->modifiers &= ~Wl_Modifier_Ctrl;
        //     }
        //     if(event->key == Wl_Key_Shift && event->modifiers & Wl_Modifier_Shift)
        //     {
        //         event->modifiers &= ~Wl_Modifier_Shift;
        //     }
        }break;

        case WM_SYSCHAR:
        {
            WORD vk_code = LOWORD(wParam);
            if (vk_code == VK_SPACE)
            {
              result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
            }
            else
            {
              result = 0;
            }
        }break;

        case WM_CHAR:
        {
        }break;

        case WM_KILLFOCUS:
        {
        }break;

        case WM_SETCURSOR:
        {
        }break;

        case WM_DPICHANGED:
        {
        }break;

            //- rjf: [file drop]
        case WM_DROPFILES:
        {
        }break;
            //- rjf: [custom border]
        case WM_NCPAINT:
        {
        }break;
        case WM_DWMCOMPOSITIONCHANGED:
        {
        }break;
        case WM_WINDOWPOSCHANGED:
        {
        }break;
        case WM_SETICON:
        case WM_SETTEXT:
        {
        }break;

        case WM_NCACTIVATE:
        {
        }break;

        case WM_NCCALCSIZE:
        {
        }break;

        case WM_NCHITTEST:
        {
        }break;
    }
    return result;
}

// Basic Window functions
//=============================================================================

internal void wl_window_open(Str8 title, Vec2I32 win_size)
{
    HINSTANCE instance = GetModuleHandleW(NULL);

    WNDCLASSEXW wc = ZERO_STRUCT;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = wl_w32_window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = L"graphical-window";
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
    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW,
        wc.lpszClassName, title16.str,
        WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        win_size.x, win_size.y,
        0, 0,
        instance, 0
    );
    if (!hwnd) {
        MessageBoxW(NULL, L"Faild to create window.", L"Error", MB_OK|MB_ICONERROR);
        os_exit(1);
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
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
