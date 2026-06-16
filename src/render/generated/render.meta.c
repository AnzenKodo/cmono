//- GENERATED CODE

C_LINKAGE_BEGIN
Str8 _render_tex2d_format_display_string_table[9] =
{
    str8_comp("R8"),
    str8_comp("RG8"),
    str8_comp("RGBA8"),
    str8_comp("BGRA8"),
    str8_comp("R16"),
    str8_comp("RGBA16"),
    str8_comp("R32"),
    str8_comp("RG32"),
    str8_comp("RGBA32"),
};

uint8_t _render_tex2d_format_bytes_per_pixel_table[9] =
{
    1,
    2,
    4,
    4,
    2,
    8,
    4,
    8,
    16,
};

Str8 _render_resource_kind_display_string_table[3] =
{
    str8_comp("Static"),
    str8_comp("Dynamic"),
    str8_comp("Stream"),
};

Str8 _render_tex2d_sample_kind_display_string_table[2] =
{
    str8_comp("Nearest"),
    str8_comp("Linear"),
};

Str8 _render_pass_kind_display_string_table[1] =
{
    str8_comp("UI"),
};

uint8_t _render_pass_kind_batch_table[1] =
{
    1,
};

uint64_t _render_pass_kind_params_size_table[1] =
{
    sizeof(Render_Pass_Params_UI),
};

C_LINKAGE_END

