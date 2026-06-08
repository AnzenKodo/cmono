// ak: Helper Functions
//=============================================================================

internal Wl_Window wl_window_zero(void)
{
    Wl_Window window = ZERO_STRUCT;
    return window;
}

internal bool wl_window_match(Wl_Window a, Wl_Window b)
{
    return a.u64[0] == b.u64[0];
}

// ak: Window Close Functions
//=============================================================================

internal void wl_exit(void)
{
    _wl_core_state.exit = true;
}

internal bool wl_should_exit(void)
{
    return _wl_core_state.exit;
}

// ak: Event Functions
//=============================================================================

internal void wl_update_events(void)
{
    // ak: update event
    _wl_core_state.event = wl_get_event();
    // ak: update FPS
    _wl_core_state.frame_count++;
    uint64_t frame_current_time = os_now_microsec();
    uint64_t delta = frame_current_time - _wl_core_state.frame_prev_time;
    if (delta >= Million(1)) {
        _wl_core_state.fps = _wl_core_state.frame_count;
        _wl_core_state.frame_count = 0;
        _wl_core_state.frame_prev_time = frame_current_time;
    }
}

internal bool wl_is_key_pressed(Wl_Key key)
{
    return _wl_core_state.event.key == key;
}

internal bool wl_is_event_happen(Wl_EventType type)
{
    return _wl_core_state.event.type == type;
}

// ak: Get Window Property
//=============================================================================

internal uint32_t wl_display_width_get(void)
{
    return _wl_core_state.display_width;
}
internal uint32_t wl_display_height_get(void)
{
    return _wl_core_state.display_height;
}

// ak: FPS
//=============================================================================

internal uint32_t wl_get_fps(void)
{
    return _wl_core_state.fps;
}

internal void wl_set_fps(uint32_t fps)
{
    os_sleep_microsec(Million(1)/fps);
}
