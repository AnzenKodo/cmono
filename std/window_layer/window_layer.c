// ak: External Includes
//=============================================================================

#define RGFW_ASSERT Assert
// #define RGFW_STATIC_ASSERT(check_name, x) AssertAlways((check_name), (x))
// #define RGFW_SNPRINTF fmt_snprintf
#define RGFW_PRINTF fmt_printf
#define RGFW_USERPTR NULL
#define RGFW_UNUSED(x) Unused(x)
#define RGFW_ABS(x) Abs(x)
#define RGFW_ROUND(x) round_f32(x)
#define RGFW_ROUNDF(x) round_f32(x)
#define RGFW_MIN(x, y) Min(x, y)
// #define RGFW_ALLOC malloc
// #define RGFW_FREE free
#define RGFW_MEMZERO(ptr, num) MemSetZero(ptr, num)
#define RGFW_MEMCPY(dist, src, len) mem_copy(dist, src, len)
// #define RGFW_STRNCMP(s1, s2, max) strncmp(s1, s2, max)
// #define RGFW_STRNCPY(dist, src, len) strncpy(dist, src, len)
// #define RGFW_STRSTR(str, substr) strstr(str, substr)
// #define RGFW_STRTOL(str, endptr, base) strtol(str, endptr, base)
// #define RGFW_ATOF(num) atof(num)
#define RGFW_OPENGL
#define RGFW_NO_X11_CURSOR
#define RGFW_IMPLEMENTATION
#define RGFW_BOOL_DEFINED
typedef bool RGFW_bool;
#undef internal
#include "vendor/RGFW.h"
#define internal static

// ak: Types
//=============================================================================

typedef struct _Wl_Window _Wl_Window;
struct _Wl_Window
{
    _Wl_Window *next;
    _Wl_Window *prev;
    RGFW_window *rgfw_window;
    Rng2_F32 rect;
    RGFW_surface *surface;
    bool is_fullscreen;
};

typedef struct _Wl_State _Wl_State;
struct _Wl_State {
    Arena *arena;
    bool   exit;
    _Wl_Window *free_window;
    _Wl_Window *first_window;
    _Wl_Window *last_window;
    Wl_Cursor last_set_cursor;
};

global _Wl_State *_wl_state = STRUCT_ZERO;

// ak: Private Function Definition
//=============================================================================

//- ak: Helper ================================================================

internal _Wl_Window *_wl_rgfw_window_from_rgfw(RGFW_window *window);
internal Wl_Key _wl_rgfw_key_from_rgfw(RGFW_key key, bool *is_right_sided);
internal Wl_Modifiers _wl_rgfw_modifiers_from_rgfw(RGFW_keymod mod);
internal RGFW_mouseIcon _wl_rgfw_mouse_icon_from_cursor(Wl_Cursor cursor);

// ak: Function Implementation
//=============================================================================

//- ak: Helper ================================================================

internal _Wl_Window *_wl_rgfw_window_from_rgfw(RGFW_window *win)
{
    if (win == NULL) { return NULL; }
    _Wl_Window *window = (_Wl_Window *)RGFW_window_getUserPtr(win);
    if (window != NULL)
    {
        return window;
    }
    if (_wl_state != NULL)
    {
        for (_Wl_Window *w = _wl_state->first_window; w != NULL; w = w->next)
        {
            if (w->rgfw_window == win)
            {
                return w;
            }
        }
    }
    return NULL;
}

internal Wl_Key _wl_rgfw_key_from_rgfw(RGFW_key key, bool *is_right_sided)
{
    if (is_right_sided)
    {
        *is_right_sided = false;
    }
    switch (key)
    {
        case RGFW_keyNULL:         return Wl_Key_Null;
        case RGFW_keyEscape:       return Wl_Key_Esc;
        case RGFW_keyBacktick:     return Wl_Key_Tick;
        case RGFW_key0:            return Wl_Key_0;
        case RGFW_key1:            return Wl_Key_1;
        case RGFW_key2:            return Wl_Key_2;
        case RGFW_key3:            return Wl_Key_3;
        case RGFW_key4:            return Wl_Key_4;
        case RGFW_key5:            return Wl_Key_5;
        case RGFW_key6:            return Wl_Key_6;
        case RGFW_key7:            return Wl_Key_7;
        case RGFW_key8:            return Wl_Key_8;
        case RGFW_key9:            return Wl_Key_9;
        case RGFW_keyMinus:        return Wl_Key_Minus;
        case RGFW_keyEqual:        return Wl_Key_Equal;
        case RGFW_keyBackSpace:    return Wl_Key_Backspace;
        case RGFW_keyTab:          return Wl_Key_Tab;
        case RGFW_keySpace:        return Wl_Key_Space;
        case RGFW_keyA:            return Wl_Key_A;
        case RGFW_keyB:            return Wl_Key_B;
        case RGFW_keyC:            return Wl_Key_C;
        case RGFW_keyD:            return Wl_Key_D;
        case RGFW_keyE:            return Wl_Key_E;
        case RGFW_keyF:            return Wl_Key_F;
        case RGFW_keyG:            return Wl_Key_G;
        case RGFW_keyH:            return Wl_Key_H;
        case RGFW_keyI:            return Wl_Key_I;
        case RGFW_keyJ:            return Wl_Key_J;
        case RGFW_keyK:            return Wl_Key_K;
        case RGFW_keyL:            return Wl_Key_L;
        case RGFW_keyM:            return Wl_Key_M;
        case RGFW_keyN:            return Wl_Key_N;
        case RGFW_keyO:            return Wl_Key_O;
        case RGFW_keyP:            return Wl_Key_P;
        case RGFW_keyQ:            return Wl_Key_Q;
        case RGFW_keyR:            return Wl_Key_R;
        case RGFW_keyS:            return Wl_Key_S;
        case RGFW_keyT:            return Wl_Key_T;
        case RGFW_keyU:            return Wl_Key_U;
        case RGFW_keyV:            return Wl_Key_V;
        case RGFW_keyW:            return Wl_Key_W;
        case RGFW_keyX:            return Wl_Key_X;
        case RGFW_keyY:            return Wl_Key_Y;
        case RGFW_keyZ:            return Wl_Key_Z;
        case RGFW_keyPeriod:       return Wl_Key_Period;
        case RGFW_keyComma:        return Wl_Key_Comma;
        case RGFW_keySlash:        return Wl_Key_Slash;
        case RGFW_keyBracket:      return Wl_Key_LeftBracket;
        case RGFW_keyCloseBracket: return Wl_Key_RightBracket;
        case RGFW_keySemicolon:    return Wl_Key_Semicolon;
        case RGFW_keyApostrophe:   return Wl_Key_Quote;
        case RGFW_keyBackSlash:    return Wl_Key_BackSlash;
        case RGFW_keyReturn:       return Wl_Key_Return;
        case RGFW_keyDelete:       return Wl_Key_Delete;
        case RGFW_keyF1:           return Wl_Key_F1;
        case RGFW_keyF2:           return Wl_Key_F2;
        case RGFW_keyF3:           return Wl_Key_F3;
        case RGFW_keyF4:           return Wl_Key_F4;
        case RGFW_keyF5:           return Wl_Key_F5;
        case RGFW_keyF6:           return Wl_Key_F6;
        case RGFW_keyF7:           return Wl_Key_F7;
        case RGFW_keyF8:           return Wl_Key_F8;
        case RGFW_keyF9:           return Wl_Key_F9;
        case RGFW_keyF10:          return Wl_Key_F10;
        case RGFW_keyF11:          return Wl_Key_F11;
        case RGFW_keyF12:          return Wl_Key_F12;
        case RGFW_keyF13:          return Wl_Key_F13;
        case RGFW_keyF14:          return Wl_Key_F14;
        case RGFW_keyF15:          return Wl_Key_F15;
        case RGFW_keyF16:          return Wl_Key_F16;
        case RGFW_keyF17:          return Wl_Key_F17;
        case RGFW_keyF18:          return Wl_Key_F18;
        case RGFW_keyF19:          return Wl_Key_F19;
        case RGFW_keyF20:          return Wl_Key_F20;
        case RGFW_keyF21:          return Wl_Key_F21;
        case RGFW_keyF22:          return Wl_Key_F22;
        case RGFW_keyF23:          return Wl_Key_F23;
        case RGFW_keyF24:          return Wl_Key_F24;
        case RGFW_keyCapsLock:     return Wl_Key_CapsLock;
        case RGFW_keyShiftL:       return Wl_Key_ShiftLeft;
        case RGFW_keyShiftR:       if (is_right_sided) *is_right_sided = true; return Wl_Key_ShiftRight;
        case RGFW_keyControlL:     return Wl_Key_CtrlLeft;
        case RGFW_keyControlR:     if (is_right_sided) *is_right_sided = true; return Wl_Key_CtrlRight;
        case RGFW_keyAltL:         return Wl_Key_AltLeft;
        case RGFW_keyAltR:         if (is_right_sided) *is_right_sided = true; return Wl_Key_AltRight;
        case RGFW_keySuperL:       return Wl_Key_SuperLeft;
        case RGFW_keySuperR:       if (is_right_sided) *is_right_sided = true; return Wl_Key_SuperRight;
        case RGFW_keyUp:           return Wl_Key_Up;
        case RGFW_keyDown:         return Wl_Key_Down;
        case RGFW_keyLeft:         return Wl_Key_Left;
        case RGFW_keyRight:        return Wl_Key_Right;
        case RGFW_keyInsert:       return Wl_Key_Insert;
        case RGFW_keyMenu:         return Wl_Key_Menu;
        case RGFW_keyEnd:          return Wl_Key_End;
        case RGFW_keyHome:         return Wl_Key_Home;
        case RGFW_keyPageUp:       return Wl_Key_PageUp;
        case RGFW_keyPageDown:     return Wl_Key_PageDown;
        case RGFW_keyNumLock:      return Wl_Key_NumLock;
        case RGFW_keyPadSlash:     return Wl_Key_NumSlash;
        case RGFW_keyPadMultiply:  return Wl_Key_NumStar;
        case RGFW_keyPadPlus:      return Wl_Key_NumPlus;
        case RGFW_keyPadMinus:     return Wl_Key_NumMinus;
        case RGFW_keyPadPeriod:    return Wl_Key_NumPeriod;
        case RGFW_keyPadReturn:    return Wl_Key_NumReturn;
        case RGFW_keyPad0:         return Wl_Key_Num0;
        case RGFW_keyPad1:         return Wl_Key_Num1;
        case RGFW_keyPad2:         return Wl_Key_Num2;
        case RGFW_keyPad3:         return Wl_Key_Num3;
        case RGFW_keyPad4:         return Wl_Key_Num4;
        case RGFW_keyPad5:         return Wl_Key_Num5;
        case RGFW_keyPad6:         return Wl_Key_Num6;
        case RGFW_keyPad7:         return Wl_Key_Num7;
        case RGFW_keyPad8:         return Wl_Key_Num8;
        case RGFW_keyPad9:         return Wl_Key_Num9;
        case RGFW_keyScrollLock:   return Wl_Key_ScrollLock;
        case RGFW_keyPrintScreen:  return Wl_Key_Print;
        case RGFW_keyPause:        return Wl_Key_Pause;
        default:                   return Wl_Key_Null;
    }
}

internal Wl_Modifiers _wl_rgfw_modifiers_from_rgfw(RGFW_keymod mod)
{
    Wl_Modifiers result = Wl_Modifier_None;
    if (mod & RGFW_modControl) result = (Wl_Modifiers)(result | Wl_Modifier_Ctrl);
    if (mod & RGFW_modShift)   result = (Wl_Modifiers)(result | Wl_Modifier_Shift);
    if (mod & RGFW_modAlt)     result = (Wl_Modifiers)(result | Wl_Modifier_Alt);
    if (mod & RGFW_modSuper)   result = (Wl_Modifiers)(result | Wl_Modifier_Window);
    return result;
}

internal RGFW_mouseIcon _wl_rgfw_mouse_icon_from_cursor(Wl_Cursor cursor)
{
    switch (cursor)
    {
        case Wl_Cursor_Pointer:         return RGFW_mouseArrow;
        case Wl_Cursor_IBar:            return RGFW_mouseIbeam;
        case Wl_Cursor_LeftRight:       return RGFW_mouseResizeEW;
        case Wl_Cursor_UpDown:          return RGFW_mouseResizeNS;
        case Wl_Cursor_DownRight:       return RGFW_mouseResizeNWSE;
        case Wl_Cursor_UpRight:         return RGFW_mouseResizeNESW;
        case Wl_Cursor_UpDownLeftRight: return RGFW_mouseResizeAll;
        case Wl_Cursor_HandPoint:       return RGFW_mousePointingHand;
        case Wl_Cursor_Disabled:        return RGFW_mouseNotAllowed;
        default:                        return RGFW_mouseNormal;
    }
}

internal inline bool wl_window_match(Wl_Window a, Wl_Window b)
{
    return a.id[0] == b.id[0];
}

internal inline bool wl_window_is_zero(Wl_Window window)
{
    return wl_window_match(window, StructZeroType(Wl_Window));
}

internal Wl_Event *wl_event_list_push_new(Arena *arena, Wl_Event_List *evts, Wl_Event_Kind kind)
{
    Wl_Event *evt = arena_push(arena, Wl_Event, 1);
    DLLPushBack(evts->first, evts->last, evt);
    evts->length += 1;
    evt->timestamp_us = time_now_us();
    evt->kind = kind;
    return evt;
}

//- ak: Basic Window Functions ================================================

internal void wl_init(void)
{
    Arena *arena = arena_alloc();
    _wl_state = arena_push(arena, _Wl_State, 1);
    _wl_state->arena = arena;
    RGFW_init("window", RGFW_initOpenGL);
    static RGFW_glHints hints;
    hints = (RGFW_glHints)RGFW_DEFAULT_GL_HINTS;
    hints.major = 3;
    hints.minor = 3;
    hints.profile = RGFW_glCore;
    hints.sRGB = 1;
    hints.doubleBuffer = RGFW_TRUE;
    hints.depth = 24;
    hints.stencil = 8;
    RGFW_setGlobalHints_OpenGL(&hints);
}

internal void wl_uninit(void)
{
    if (_wl_state == NULL) { return; }
    while (_wl_state->first_window != NULL)
    {
        Wl_Window window = HandlePack(Wl_Window, _wl_state->first_window);
        wl_window_close(window);
    }
    RGFW_deinit();
    if (_wl_state->arena != NULL)
    {
        arena_free(_wl_state->arena);
    }
    _wl_state = NULL;
}

internal Wl_Window wl_window_open(Str8 title)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    _Wl_Window *_window = _wl_state->free_window;
    if (_window != NULL)
    {
        SLLStackPop(_wl_state->free_window);
    }
    else
    {
        _window = arena_push_nz(_wl_state->arena, _Wl_Window, 1);
    }
    MemSetZeroStruct(_window);
    DLLPushBack(_wl_state->first_window, _wl_state->last_window, _window);
    
    Str8 title_copy = str8_copy(scratch.arena, title);
    char *title_cstr = (char *)title_copy.cstr;
    
    RGFW_window *rgfw_window = RGFW_createWindow(title_cstr, 0, 0, 800, 600, RGFW_windowAllowDND | RGFW_windowOpenGL);
    
    _window->rgfw_window = rgfw_window;
    RGFW_window_setUserPtr(rgfw_window, _window);
    
    arena_scratch_end(scratch);
    Wl_Window window = HandlePack(Wl_Window, _window);
    return window;
}

internal void wl_window_close(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *w = HandleUnpack(_Wl_Window, window);
    if (w != NULL && w->rgfw_window != NULL)
    {
        if (w->surface != NULL)
        {
            RGFW_surface_free(w->surface);
            w->surface = NULL;
        }
        RGFW_window_close(w->rgfw_window);
        w->rgfw_window = NULL;
        DLLRemove(_wl_state->first_window, _wl_state->last_window, w);
        SLLStackPush(_wl_state->free_window, w);
    }
}

internal void wl_window_make_current(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window != NULL && _window->rgfw_window != NULL)
    {
        RGFW_window_makeCurrentContext_OpenGL(_window->rgfw_window);
    }
}

internal void wl_window_swap_buffers(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window != NULL && _window->rgfw_window != NULL)
    {
        RGFW_window_swapBuffers_OpenGL(_window->rgfw_window);
    }
}

//- ak: Window Exit ===========================================================

internal void wl_exit(void)
{
    _wl_state->exit = true;
}

internal bool wl_should_exit(void)
{
    return _wl_state->exit;
}

//- ak: Window Event ==========================================================

internal void wl_update_events(void)
{
    RGFW_pollEvents();
}

internal Wl_Event_List wl_get_events(Arena *arena, bool wait)
{
    Wl_Event_List events = STRUCT_ZERO;
    
    for (;;)
    {
        RGFW_event event;
        bool got_event = false;
        
        if (RGFW_checkEvent(&event))
        {
            got_event = true;
        }
        else if (wait && events.length == 0)
        {
            RGFW_waitForEvent(-1);
            if (RGFW_checkEvent(&event))
            {
                got_event = true;
            }
        }
        
        while (got_event)
        {
            _Wl_Window *_window = _wl_rgfw_window_from_rgfw(event.common.win);
            Wl_Window window = HandlePack(Wl_Window, _window);
            
            switch (event.type)
            {
                case RGFW_keyPressed:
                case RGFW_keyReleased:
                {
                    bool is_right_sided = false;
                    Wl_Key key = _wl_rgfw_key_from_rgfw(event.key.value, &is_right_sided);
                    Wl_Modifiers modifiers = _wl_rgfw_modifiers_from_rgfw(event.key.mod);
                    
                    Wl_Event *e = wl_event_list_push_new(arena, &events,
                        (event.type == RGFW_keyPressed) ? Wl_Event_Kind_Press : Wl_Event_Kind_Release);
                    e->window = window;
                    e->modifiers = modifiers;
                    e->key = key;
                    e->is_repeat = (bool)event.key.repeat;
                    e->right_sided = is_right_sided;
                } break;
                
                case RGFW_keyChar:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_Text);
                    e->window = window;
                    e->character = event.keyChar.value;
                } break;
                
                case RGFW_mouseButtonPressed:
                case RGFW_mouseButtonReleased:
                {
                    Wl_Key key = Wl_Key_Null;
                    switch (event.button.value)
                    {
                        case RGFW_mouseLeft:   key = Wl_Key_LeftMouseButton;   break;
                        case RGFW_mouseMiddle: key = Wl_Key_MiddleMouseButton; break;
                        case RGFW_mouseRight:  key = Wl_Key_RightMouseButton;  break;
                        default: break;
                    }
                    if (key != Wl_Key_Null)
                    {
                        Wl_Event *e = wl_event_list_push_new(arena, &events,
                            (event.type == RGFW_mouseButtonPressed) ? Wl_Event_Kind_Press : Wl_Event_Kind_Release);
                        e->window = window;
                        e->key = key;
                        if (_window != NULL && _window->rgfw_window != NULL)
                        {
                            i32 mx = 0, my = 0;
                            RGFW_window_getMouse(_window->rgfw_window, &mx, &my);
                            e->pos = (Vec2_F32){ (float)mx, (float)my };
                        }
                    }
                } break;
                
                case RGFW_mouseScroll:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_Scroll);
                    e->window = window;
                    e->delta = (Vec2_F32){ event.delta.x, event.delta.y };
                    if (_window != NULL && _window->rgfw_window != NULL)
                    {
                        i32 mx = 0, my = 0;
                        RGFW_window_getMouse(_window->rgfw_window, &mx, &my);
                        e->pos = (Vec2_F32){ (float)mx, (float)my };
                    }
                } break;
                
                case RGFW_mouseMotion:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_MouseMove);
                    e->window = window;
                    e->pos = (Vec2_F32){ (float)event.mouse.x, (float)event.mouse.y };
                } break;
                
                case RGFW_windowFocusOut:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_WindowLoseFocus);
                    e->window = window;
                } break;
                
                case RGFW_windowMoved:
                case RGFW_windowResized:
                {
                    if (_window != NULL && _window->rgfw_window != NULL)
                    {
                        _window->rect = rng2p(
                            (float)_window->rgfw_window->x,
                            (float)_window->rgfw_window->y,
                            (float)(_window->rgfw_window->x + _window->rgfw_window->w),
                            (float)(_window->rgfw_window->y + _window->rgfw_window->h)
                        );
                    }
                    if (event.type == RGFW_windowResized)
                    {
                        Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_WindowResize);
                        e->window = window;
                    }
                } break;
                
                case RGFW_windowClose:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_WindowClose);
                    e->window = window;
                } break;
                
                case RGFW_dataDrop:
                {
                    Wl_Event *e = wl_event_list_push_new(arena, &events, Wl_Event_Kind_FileDrop);
                    e->window = window;
                    for (const RGFW_dataDropNode *node = event.drop.value; node != NULL; node = node->next)
                    {
                        if (node->data != NULL)
                        {
                            str8_list_push(arena, &e->strings, str8_from_cstr((char *)node->data));
                        }
                    }
                } break;
                
                default: break;
            }
            
            got_event = RGFW_checkQueuedEvent(&event);
        }
        
        if (events.length > 0 || !wait)
        {
            break;
        }
    }
    
    return events;
}

internal void wl_set_fps(uint64_t fps)
{
    os_sleep_us(Million(1)/fps);
}

//- ak: Window Property =======================================================

internal Rng2_F32 wl_rect_from_window(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return rng2p(0.0f, 0.0f, 0.0f, 0.0f); }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window == NULL || _window->rgfw_window == NULL)
    {
        return (Rng2_F32){0, 0, 0, 0};
    }
    RGFW_window *rw = _window->rgfw_window;
    _window->rect = rng2p((float)rw->x, (float)rw->y, (float)(rw->x + rw->w), (float)(rw->y + rw->h));
    return _window->rect;
}

internal Rng2_F32 wl_canvas_rect_from_window(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return rng2p(0.0f, 0.0f, 0.0f, 0.0f); }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window == NULL || _window->rgfw_window == NULL)
    {
        return (Rng2_F32){0, 0, 0, 0};
    }
    return rng2p(0.f, 0.f, (float)_window->rgfw_window->w, (float)_window->rgfw_window->h);
}

internal Rng2_F32 wl_display_rect(void)
{
    Rng2_F32 display_rect = rng2p(0.f, 0.f, 0.f, 0.f);
    RGFW_monitor *mon = RGFW_getPrimaryMonitor();
    if (mon != NULL)
    {
        display_rect = rng2p(0.f, 0.f, (float)mon->mode.w, (float)mon->mode.h);
    }
    return display_rect;
}

internal void wl_window_pos_set(Wl_Window window, size_t x, size_t y)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window != NULL && _window->rgfw_window != NULL)
    {
        RGFW_window_move(_window->rgfw_window, (i32)x, (i32)y);
    }
}

internal void wl_window_border_set(Wl_Window window, bool enable)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window != NULL && _window->rgfw_window != NULL)
    {
        RGFW_window_setBorder(_window->rgfw_window, enable ? RGFW_TRUE : RGFW_FALSE);
    }
}

internal bool wl_window_is_fullscreen(Wl_Window window)
{
    if (wl_window_is_zero(window)) { return false; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window == NULL || _window->rgfw_window == NULL) { return false; }
    return (bool)RGFW_window_isFullscreen(_window->rgfw_window);
}

internal void wl_window_set_fullscreen(Wl_Window window, bool fullscreen)
{
    if (wl_window_is_zero(window)) { return; }
    _Wl_Window *_window = HandleUnpack(_Wl_Window, window);
    if (_window != NULL && _window->rgfw_window != NULL)
    {
        _window->is_fullscreen = fullscreen;
        RGFW_window_setFullscreen(_window->rgfw_window, fullscreen ? RGFW_fullscreenBorderless : RGFW_fullscreenNone);
    }
}

