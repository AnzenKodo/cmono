fn void wl_window_open(Str8 title, U32 win_width, U32 win_height)
{
    Display *display = XOpenDisplay(NULL);
    XVisualInfo visual_info = ZERO_STRUCT;
    Window window = window = XCreateWindow(
        display,
        RootWindow(display, visual_info.screen),
        0, 0, win_width, win_height, 0,
        visual_info.depth, InputOutput, visual_info.visual,
        CWColormap|CWEventMask, &set_window_attributes
    );
}

void game_entry_point(void) {

}

int main(void)
{
    game_entry_point();
}
