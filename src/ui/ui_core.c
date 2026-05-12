internal UI_State *ui_state_alloc(Arena *arena)
{
    UI_State *state = arena_push(arena, UI_State, 1);
    state->box_table_size = 4096;
    return state;
}
    
internal void ui_state_select(UI_State *state)
{
    ui_state = state;
}


internal UI_Key ui_key_zero(void)
{
    UI_Key result = {0};
    return result;
}

internal UI_Key ui_key_make(uint64_t v)
{
    UI_Key result = {v};
    return result;
}

internal bool ui_key_match(UI_Key a, UI_Key b)
{
  return a.u64[0] == b.u64[0];
}

internal Str8 ui_hash_part_from_key_string(Str8 string)
{
    Str8 result = string;
    // ak: look for ### patterns, which can replace the entirety of the part of
    // the string that is hashed.
    uint64_t hash_replace_signifier_pos = str8_find_substr(string, 0, str8("###"), 0);
    if(hash_replace_signifier_pos < string.size)
    {
        result = str8_skip(string, hash_replace_signifier_pos);
    }
    return result;
}

internal UI_Key ui_key_from_string(UI_Key seed, Str8 string)
{
    UI_Key result = ZERO_STRUCT;
    if (string.size != 0)
    {
        Str8 hash_part = ui_hash_part_from_key_string(string);
        result.u64[0] = u64_hash_from_seed_str8(seed.u64[0], hash_part);
    }
    return result;
}


internal UI_Key ui_key_from_stringf(UI_Key seed, char *fmt, ...)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    va_list args;
    va_start(args, fmt);
    Str8 string = str8fv(scratch.arena, fmt, args);
    va_end(args);
    UI_Key key = ui_key_from_string(seed, string);
    arena_scratch_end(scratch);
    return key;
}

internal bool ui_box_is_nil(UI_Box *box)
{
    return box == 0 || box == &ui_box_nil;
}

internal UI_Box *ui_box_from_key(UI_Key key)
{
    UI_Box *result = &ui_box_nil;
    if(!ui_key_match(key, ui_key_zero()))
    {
        uint64_t slot = key.u64[0] % ui_state->box_table_size;
        for(UI_Box *b = ui_state->box_table[slot].hash_first; !ui_box_is_nil(b); b = b->hash_next)
        {
            if(ui_key_match(b->key, key))
            {
                result = b;
                break;
            }
        }
    }
    return result;
}

internal UI_Box *ui_box_make_from_key(UI_Box_Flags flags, UI_Key key)
{
    UI_Box *box = ui_box_from_key(key);
    if (ui_box_is_nil(box))
    {
    }
}

internal UI_Box *ui_box_make(UI_Box_Flags flags, Str8 string)
{
    //- ak: produce a key from the string
    Str8 string_hash_part = ui_hash_part_from_key_string(string);
    UI_Key key = ui_key_from_string(ui_key_zero(), string_hash_part);
    //- ak: build the box from the key
    UI_Box *box = ui_box_make_from_key(flags, key);
    return box;
}
    
internal UI_Box *ui_box_makef(UI_Box_Flags flags, char *fmt, ...)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    va_list args;
    va_start(args, fmt);
    Str8 string = str8fv(scratch.arena, fmt, args);
    UI_Box *result = ui_box_make(flags, string);
    va_end(args);
    arena_scratch_end(scratch);
    return result;
}

internal void ui_build_begin(void)
{
    // ak: build root
    UI_Box *root = ui_box_makef(0, "window_root_###%I64x", 324);
}
