// ak: Helper functions
//=============================================================================

internal _Wl_X11_Window *_wl_x11_window_from_xwindow(xcb_window_t xwindow)
{
    _Wl_X11_Window *result = 0;
    for (_Wl_X11_Window *window = _wl_x11_state->first_window;
        window != 0;
        window = window->next)
    {
        if (window->xwindow == xwindow)
        {
            result = window;
            break;
        }
    }
    return result;
}

// ak: Basic window functions
//=============================================================================

internal void wl_init(void)
{
    // ak: initialize basics
    Arena *arena = arena_alloc();
    _wl_x11_state = arena_push(arena, _Wl_X11_State, 1);
    _wl_x11_state->arena = arena;
    
    // ak: setup connection
    _wl_x11_state->connection = xcb_connect(NULL, NULL);
    _wl_x11_state->screen = xcb_setup_roots_iterator(xcb_get_setup(_wl_x11_state->connection)).data;
    
    // ak: calculate atoms
    WlX11LoadAtom(_wl_x11_state, WM_PROTOCOLS);
    WlX11LoadAtom(_wl_x11_state, WM_DELETE_WINDOW);
    _wl_x11_state->wm_protocols = WM_PROTOCOLS;
    _wl_x11_state->wm_delete_window = WM_DELETE_WINDOW;
}

internal Wl_Window wl_window_open(Str8 title, size_t width, size_t height)
{
    _Wl_X11_Window *window_os = _wl_x11_state->free_window;
    if (window_os)
    {
        SLLStackPop(_wl_x11_state->free_window);
    }
    else
    {
        window_os = arena_push_nz(_wl_x11_state->arena, _Wl_X11_Window, 1);
    }
    MemSetZeroStruct(window_os);
    DLLPushBack(_wl_x11_state->first_window, _wl_x11_state->last_window, window_os);
    
    window_os->xwindow = xcb_generate_id(_wl_x11_state->connection);
    
    // ak: create window
    int mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    int value_list[] = {
        _wl_x11_state->screen->black_pixel,
		XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE |
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
    };
    xcb_create_window(
        _wl_x11_state->connection, XCB_COPY_FROM_PARENT, window_os->xwindow, _wl_x11_state->screen->root,
        0, 0, width, height, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, _wl_x11_state->screen->root_visual, mask, value_list
    );
    
    // ak: set window title
    xcb_change_property(
        _wl_x11_state->connection,
        XCB_PROP_MODE_REPLACE, window_os->xwindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        8, title.length, title.cstr
    );
    xcb_change_property(
        _wl_x11_state->connection,
        XCB_PROP_MODE_REPLACE, window_os->xwindow, XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING,
        8, title.length, title.cstr
    );
    // xcb_change_property(
    //     _wl_x11_state->connection, XCB_PROP_MODE_REPLACE, window->xwindow, XCB_ATOM_WM_CLASS,
    //     XCB_ATOM_STRING, 8, sizeof("title""\0""Title"), "title\0Title"
    // );
    
    // ak: handle close event
    xcb_change_property(
        _wl_x11_state->connection, XCB_PROP_MODE_REPLACE, window_os->xwindow, _wl_x11_state->wm_protocols,
        XCB_ATOM_ATOM, 32, 1, &_wl_x11_state->wm_delete_window
    );
    
    // ak: map window
    xcb_map_window(_wl_x11_state->connection, window_os->xwindow);
    xcb_flush(_wl_x11_state->connection);
    
    // ak: get display size
    _wl_core_state.display_width = _wl_x11_state->screen->width_in_pixels;
    _wl_core_state.display_height = _wl_x11_state->screen->height_in_pixels;
    window_os->rect = rng2p(0.0f, 0.0f, (float)width, (float)height);
    window_os->canvas_rect = rng2p(0.0f, 0.0f, (float)width, (float)height);
    _wl_core_state.frame_prev_time = os_now_microsec();
     
    // ak: convert to handle & return
    Wl_Window window = {(uint64_t)window_os};
    return window;
}

internal void wl_window_close(void)
{
    xcb_disconnect(_wl_x11_state->connection);
}

// ak: Event Functions
//=============================================================================

internal Wl_Event wl_get_event(void)
{
    Wl_Event event = ZERO_STRUCT;
    xcb_generic_event_t *xcb_event;
    while ((xcb_event = xcb_poll_for_event(_wl_x11_state->connection)))
    {
        switch (xcb_event->response_type & ~0x80)
        {
            // ak: keyboard key presses/releases
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
            {
                xcb_key_release_event_t *key_event = (xcb_key_release_event_t *)xcb_event;
                // ak: determine mod_key
                Wl_ModKey mod_key = (Wl_ModKey)0;
                if (key_event->state & XCB_MOD_MASK_SHIFT)
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Shift);
                }
                if (key_event->state & XCB_MOD_MASK_CONTROL)
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Ctrl);
                }
                if (key_event->state & XCB_MOD_MASK_1)
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Alt);
                }
                // ak: assign keys
                Wl_Key key = Wl_Key_Null;
                switch(key_event->detail)
                {
                    // NOTE(ak): to get keycodes run `xmodmap -pk`
                    case 9:{key = Wl_Key_Esc;};break;
                    case 10:{key = Wl_Key_1;}break;
                    case 11:{key = Wl_Key_2;}break;
                    case 12:{key = Wl_Key_3;}break;
                    case 13:{key = Wl_Key_4;}break;
                    case 14:{key = Wl_Key_5;}break;
                    case 15:{key = Wl_Key_6;}break;
                    case 16:{key = Wl_Key_7;}break;
                    case 17:{key = Wl_Key_8;}break;
                    case 18:{key = Wl_Key_9;}break;
                    case 19:{key = Wl_Key_0;}break;
                    case 20:{key = Wl_Key_Minus;}break;
                    case 21:{key = Wl_Key_Equal;}break;
                    case 22:{key = Wl_Key_Backspace;}break;
                    case 23:{key = Wl_Key_Tab;}break;
                    case 24:{key = Wl_Key_Q;}break;
                    case 25:{key = Wl_Key_W;}break;
                    case 26:{key = Wl_Key_E;}break;
                    case 27:{key = Wl_Key_R;}break;
                    case 28:{key = Wl_Key_T;}break;
                    case 29:{key = Wl_Key_Y;}break;
                    case 30:{key = Wl_Key_U;}break;
                    case 31:{key = Wl_Key_I;}break;
                    case 32:{key = Wl_Key_O;}break;
                    case 33:{key = Wl_Key_P;}break;
                    case 34:{key = Wl_Key_LeftBracket;}break;
                    case 35:{key = Wl_Key_RightBracket;}break;
                    case 36:{key = Wl_Key_Return;}break;
                    case 37:{key = Wl_Key_CtrlLeft;}break;
                    case 38:{key = Wl_Key_A;}break;
                    case 39:{key = Wl_Key_S;}break;
                    case 40:{key = Wl_Key_D;}break;
                    case 41:{key = Wl_Key_F;}break;
                    case 42:{key = Wl_Key_G;}break;
                    case 43:{key = Wl_Key_H;}break;
                    case 44:{key = Wl_Key_J;}break;
                    case 45:{key = Wl_Key_K;}break;
                    case 46:{key = Wl_Key_L;}break;
                    case 47:{key = Wl_Key_Semicolon;}break;
                    case 48:{key = Wl_Key_Quote;}break;
                    case 49:{key = Wl_Key_Tick;}break;
                    case 50:{key = Wl_Key_ShiftLeft;}break;
                    case 51:{key = Wl_Key_BackSlash;}break;
                    case 52:{key = Wl_Key_Z;}break;
                    case 53:{key = Wl_Key_X;}break;
                    case 54:{key = Wl_Key_C;}break;
                    case 55:{key = Wl_Key_V;}break;
                    case 56:{key = Wl_Key_B;}break;
                    case 57:{key = Wl_Key_N;}break;
                    case 58:{key = Wl_Key_M;}break;
                    case 59:{key = Wl_Key_Comma;}break;
                    case 60:{key = Wl_Key_Period;}break;
                    case 61:{key = Wl_Key_Slash;}break;
                    case 62:{key = Wl_Key_ShiftRight;}break;
                    case 63:{key = Wl_Key_NumStar;}break;
                    case 64:{key = Wl_Key_AltLeft;}break;
                    case 65:{key = Wl_Key_Space;}break;
                    case 66:{key = Wl_Key_CapsLock;}break;
                    case 67:{key = Wl_Key_F1;}break;
                    case 68:{key = Wl_Key_F2;}break;
                    case 69:{key = Wl_Key_F3;}break;
                    case 70:{key = Wl_Key_F4;}break;
                    case 71:{key = Wl_Key_F5;}break;
                    case 72:{key = Wl_Key_F6;}break;
                    case 73:{key = Wl_Key_F7;}break;
                    case 74:{key = Wl_Key_F8;}break;
                    case 75:{key = Wl_Key_F9;}break;
                    case 76:{key = Wl_Key_F10;}break;
                    case 77:{key = Wl_Key_NumLock;}break;
                    case 78:{key = Wl_Key_ScrollLock;}break;
                    case 79:{key = Wl_Key_Num7;}break;
                    case 80:{key = Wl_Key_Num8;}break;
                    case 81:{key = Wl_Key_Num9;}break;
                    case 82:{key = Wl_Key_NumMinus;}break;
                    case 83:{key = Wl_Key_Num4;}break;
                    case 84:{key = Wl_Key_Num5;}break;
                    case 85:{key = Wl_Key_Num6;}break;
                    case 86:{key = Wl_Key_NumPlus;}break;
                    case 87:{key = Wl_Key_Num1;}break;
                    case 88:{key = Wl_Key_Num2;}break;
                    case 89:{key = Wl_Key_Num3;}break;
                    case 90:{key = Wl_Key_Num0;}break;
                    case 91:{key = Wl_Key_NumPeriod;}break;
                    // 92 ISO_Level3_Shift
                    // 93
                    // 94 less
                    case 95:{key = Wl_Key_F11;}break;
                    case 96:{key = Wl_Key_F12;}break;
                    // 97
                    // 98 Katakana
                    // 99 Hiragana
                    // 100 Henkan_Mode
                    // 101 Hiragana_Katakana
                    // 102 Muhenkan
                    // 103
                    case 104:{key = Wl_Key_NumReturn;}break;
                    case 105:{key = Wl_Key_CtrlRight;}break;
                    case 106:{key = Wl_Key_NumSlash;}break;
                    case 107:{key = Wl_Key_Print;}break;
                    case 108:{key = Wl_Key_AltRight;}break;
                    // 109 Linefeed
                    case 110:{key = Wl_Key_Home;}break;
                    case 111:{key = Wl_Key_Up;}break;
                    case 112:{key = Wl_Key_PageUp;}break;
                    case 113:{key = Wl_Key_Left;}break;
                    case 114:{key = Wl_Key_Right;}break;
                    case 115:{key = Wl_Key_End;}break;
                    case 116:{key = Wl_Key_Down;}break;
                    case 117:{key = Wl_Key_PageDown;}break;
                    case 118:{key = Wl_Key_Insert;}break;
                    case 119:{key = Wl_Key_Delete;}break;
                    // 120
                    // case 121:{key = Wl_Key_AudioMute;}break;
                    // case 122:{key = Wl_Key_AudioLowerVolume;}break;
                    // case 123:{key = Wl_Key_AudioRaiseVolume;}break;
                    // 124 XF86PowerOff
                    // 125 KP_Equal
                    // 126 plusminus
                    case 127:{key = Wl_Key_Pause;}break;
                    // 128 XF86LaunchA
                    // 129 KP_Decimal
                    // 130 Hangul
                    // 131 Hangul_Hanja
                    // 132
                    case 133:{key = Wl_Key_SuperLeft;}break;
                    case 134:{key = Wl_Key_SuperRight;}break;
                    case 135:{key = Wl_Key_Menu;}break;
                    default:{}break;
                }
                // ak: add to event variable
                if (key_event->response_type == XCB_KEY_PRESS)
                {
                    event.type = Wl_EventType_Press;
                } else {
                    event.type = Wl_EventType_Release;
                }
                event.key = key;
                event.mod_key = mod_key;
            } break;
            // ak: mouse button presses/releases
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
            {
                xcb_button_release_event_t *button_event = (xcb_button_release_event_t *)xcb_event;
                // ak: determine mod_key
                Wl_ModKey mod_key = (Wl_ModKey)0;
                if (button_event->state & XCB_MOD_MASK_SHIFT)
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Shift);
                }
                if (button_event->state & XCB_MOD_MASK_CONTROL)
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Ctrl);
                }
                if (button_event->state & XCB_MOD_MASK_1) 
                {
                    mod_key = (Wl_ModKey)(mod_key | Wl_ModKey_Alt);
                }
                // ak: assign button
                Wl_Key key = Wl_Key_Null;
                switch(button_event->detail)
                {
                    case 1:{key = Wl_Key_LeftMouseButton;}break;
                    case 2:{key = Wl_Key_MiddleMouseButton;}break;
                    case 3:{key = Wl_Key_RightMouseButton;}break;
                    default:{}
                }
                // ak: add to event variable
                if (button_event->response_type == XCB_BUTTON_PRESS) 
                {
                    event.type = Wl_EventType_Press;
                } else {
                    event.type = Wl_EventType_Release;
                }
                event.mod_key = mod_key;
                event.key = key;
            } break;
            // ak: mouse motion
    		case XCB_MOTION_NOTIFY:
    		{
                xcb_button_release_event_t *motion_event = (xcb_button_release_event_t *)xcb_event;
                // ak: add to event variable
                event.type = Wl_EventType_MouseMove;
                event.pos.x = (float)motion_event->event_x;
                event.pos.y = (float)motion_event->event_x;
            } break;
            // ak: window focus/unfocus
            case XCB_FOCUS_IN: break;
            case XCB_FOCUS_OUT:
            {
                event.type = Wl_EventType_WindowLoseFocus;
            }break;
            // ak: client messages
            case XCB_CLIENT_MESSAGE:
            {
                xcb_client_message_event_t *message = (xcb_client_message_event_t *)xcb_event;
                if (message->data.data32[0] == _wl_x11_state->wm_delete_window)
                {
                    event.type = Wl_EventType_WindowClose;
                }
            }break;
            // ak: window resize
            case XCB_CONFIGURE_NOTIFY:
            {
                xcb_configure_notify_event_t *resize_event = (xcb_configure_notify_event_t *)xcb_event;
                _Wl_X11_Window *window_os = _wl_x11_window_from_xwindow(resize_event->window);
                // ak: add to event variable
                window_os->rect = rng2p(
                    (float)resize_event->x,     (float)resize_event->y,
                    (float)resize_event->width, (float)resize_event->height
                );
                window_os->canvas_rect = rng2p(
                    .0f, .0f,
                    (float)resize_event->width, (float)resize_event->height
                );
                event.type = Wl_EventType_WindowResize;
                event.window.u64[0] = (uint64_t)window_os;
            }break;
            case XCB_EXPOSE:{
            }break;
        } // switch
        free(xcb_event);
    } // while
    return event;
}

// Window property
// ============================================================================

internal Rng2_F32 wl_rect_from_window(Wl_Window window)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return (Rng2_F32){0, 0, 0, 0};}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    return window_os->rect;
}

internal Rng2_F32 wl_canvas_rect_from_window(Wl_Window window)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return (Rng2_F32){0, 0, 0, 0};}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    return window_os->canvas_rect;
}

internal void wl_window_pos_set(Wl_Window window, size_t x, size_t y)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return;}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    
    xcb_configure_window(_wl_x11_state->connection, window_os->xwindow,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
        (int[2]){ x, y });
    xcb_flush(_wl_x11_state->connection);
}

internal void wl_window_icon_set_raw(Wl_Window window, void *icon_data, size_t width, size_t height)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return;}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    
    WlX11LoadAtom(_wl_x11_state, _NET_WM_ICON);
    size_t data[2 + width * height];
    data[0] = width;
    data[1] = height;
    mem_copy(data + 2, icon_data, width * height * sizeof(size_t));
    xcb_change_property(
        _wl_x11_state->connection, XCB_PROP_MODE_REPLACE, window_os->xwindow,
        _NET_WM_ICON, XCB_ATOM_CARDINAL, 32,
        2 + width * height, data
    );
}

internal void wl_window_border_set(Wl_Window window, bool enable)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return;}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    
    WlX11LoadAtom(_wl_x11_state, _MOTIF_WM_HINTS);
	struct {
        uint32_t flags;
        uint32_t functions;
        uint32_t decorations;
        int32_t  input_mode;
        uint32_t status;
    } hints = ZERO_STRUCT;
	hints.flags = 2;
	hints.decorations = enable;
    xcb_change_property(_wl_x11_state->connection, XCB_PROP_MODE_REPLACE, window_os->xwindow, _MOTIF_WM_HINTS, _MOTIF_WM_HINTS, 32, 5, &hints);
    xcb_flush(_wl_x11_state->connection);
}

// ak: software render
// ============================================================================

internal void wl_render_init(Wl_Window window, void *buffer)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return;}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    window_os->buffer = buffer;
    
    // ak: create pixmap format
    if (!_wl_x11_state->pixmap)
    {
        _wl_x11_state->pixmap = xcb_generate_id(_wl_x11_state->connection);
        _wl_x11_state->pixmap_format = xcb_setup_pixmap_formats(
            xcb_get_setup(_wl_x11_state->connection)
        );
        int32_t pixmap_format_length = xcb_setup_pixmap_formats_length(
            xcb_get_setup(_wl_x11_state->connection)
        );
        for (int i = 0; i < pixmap_format_length; i++) 
        {
            if (
                    _wl_x11_state->pixmap_format[i].depth==24 && _wl_x11_state->pixmap_format[i].bits_per_pixel==32
               ) 
            {
                _wl_x11_state->pixmap_format += i;
            }
        }
    }
    
    // ak: connect pixmap to window
    xcb_create_pixmap(
        _wl_x11_state->connection, _wl_x11_state->screen->root_depth,
        _wl_x11_state->pixmap, window_os->xwindow,
        wl_display_width_get(), wl_display_height_get()
    );
    
    // ak: create graphics context
    if (!_wl_x11_state->gc)
    {
        _wl_x11_state->gc = xcb_generate_id(_wl_x11_state->connection);
        uint32_t gc_mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
        uint32_t gc_values[] = {
            _wl_x11_state->screen->white_pixel, _wl_x11_state->screen->black_pixel
        };
        xcb_create_gc(
            _wl_x11_state->connection, _wl_x11_state->gc, _wl_x11_state->pixmap, gc_mask, gc_values
        );
    }
    
    // ak: create image
    window_os->image = xcb_image_create(
        wl_display_width_get(), wl_display_height_get(), XCB_IMAGE_FORMAT_Z_PIXMAP,
        _wl_x11_state->pixmap_format->scanline_pad,
        _wl_x11_state->pixmap_format->depth,
        _wl_x11_state->pixmap_format->bits_per_pixel, 0,
        xcb_get_setup(_wl_x11_state->connection)->image_byte_order,
        XCB_IMAGE_ORDER_LSB_FIRST,
        window_os->buffer, sizeof(*window_os->buffer), window_os->buffer
    );
    xcb_flush(_wl_x11_state->connection);
}

internal void wl_render_deinit(void)
{
    xcb_free_pixmap(_wl_x11_state->connection, _wl_x11_state->pixmap);
    xcb_free_gc(_wl_x11_state->connection, _wl_x11_state->gc);
}

internal void wl_render_begin(Wl_Window window)
{
    if (wl_window_match(window, wl_window_zero())) 
    {return;}
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    
    xcb_image_put(
        _wl_x11_state->connection, _wl_x11_state->pixmap,
        _wl_x11_state->gc, window_os->image, 0, 0, 0
    );
    xcb_copy_area(
        _wl_x11_state->connection, _wl_x11_state->pixmap, window_os->xwindow,
        _wl_x11_state->gc, 0, 0, 0, 0,
        wl_display_width_get(), wl_display_height_get()
    );
}

internal void wl_render_end(void)
{
}
