internal bool term_is_color_allowed(void)
{
    bool result = true;

    if (os_env_is_set(str8("NO_COLOR")) || os_env_get(str8("TERM")).length < 1)
    {
        result = false;
    }

    return result;
}

internal void term_style_start(const char *style)
{
    if (term_is_color_allowed())
    {
        fmt_print(style);
    }
}
internal void term_style_end(void)
{
    fmt_print(TERM_RESET);
}

internal char *term_style_get(const char *style)
{
    char *result = NULL;
    if (term_is_color_allowed())
    {
        result = (char *)style;
    }
    return result;
}
