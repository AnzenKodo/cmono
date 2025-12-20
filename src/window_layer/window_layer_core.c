// Window Close Functions
//=============================================================================

internal void wl_set_window_close(void)
{
    _wl_core_state.win_should_close = true;
}

internal bool wl_should_window_close(void)
{
    return _wl_core_state.win_should_close;
}

// Event Functions
//=============================================================================

internal void wl_update_events(void)
{
    // Update Event ===========================================================
    _wl_core_state.event = wl_get_event();
    // Update FPS =============================================================
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

// Get Window Property
//=============================================================================

internal uint32_t wl_get_display_width(void)
{
    return (uint32_t)_wl_core_state.display_size.x;
}

internal uint32_t wl_get_display_height(void)
{
    return (uint32_t)_wl_core_state.display_size.y;
}

internal uint32_t wl_get_window_width(void)
{
   return (uint32_t)_wl_core_state.win_size.x;
}

internal uint32_t wl_get_window_height(void)
{
    return (uint32_t)_wl_core_state.win_size.y;
}

// FPS
//=============================================================================

internal uint32_t wl_get_fps(void)
{
    return _wl_core_state.fps;
}

internal void wl_set_fps(uint32_t fps)
{
    os_sleep_microsec(Million(1)/fps);
}
