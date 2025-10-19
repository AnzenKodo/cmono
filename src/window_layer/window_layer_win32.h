#ifndef WINDOW_LAYER_W32_H
#define WINDOW_LAYER_W32_H

// Types
//=============================================================================

typedef struct Wl_Win32_State Wl_Win32_State;
struct Wl_Win32_State
{
    HWND handle;
    HINSTANCE instance;
    bool window_close;
    bool window_resize;
    Wl_Event event;

    // NOTE(aman.v): For software render
    HBITMAP bitmap;
    BITMAPINFO bitmap_info;
    HDC hdc;
    PAINTSTRUCT paint;
    void *render_buffer;
};

// Functions
//=============================================================================

internal Wl_Key os_w32_os_key_from_vkey(WPARAM vkey);
internal WPARAM os_w32_vkey_from_os_key(Wl_Key key);
internal LRESULT CALLBACK wl_w32_window_proc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);

// Global Variables
//=============================================================================

global Wl_Win32_State wl_win32_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_W32_H
