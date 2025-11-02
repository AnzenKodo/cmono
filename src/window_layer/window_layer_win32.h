#ifndef WINDOW_LAYER_W32_H
#define WINDOW_LAYER_W32_H

// Types
//=============================================================================

typedef struct _Wl_Win32_State _Wl_Win32_State;
struct _Wl_Win32_State
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

// Helper Functions ===========================================================

internal Wl_Key _os_win32_os_key_from_vkey(WPARAM vkey);
internal WPARAM _os_win32_vkey_from_os_key(Wl_Key key);
internal LRESULT CALLBACK _wl_win32_window_proc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param);

// Global variables
//=============================================================================

global _Wl_Win32_State _wl_win32_state = ZERO_STRUCT;

#endif // WINDOW_LAYER_W32_H
