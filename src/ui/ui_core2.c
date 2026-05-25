//~ ak: Generated Code
//=============================================================================

#include "./generated/ui.meta.h"

internal UI_Box ui_box(void)
{
    UI_Box box = ZERO_STRUCT;
    return box;
}

internal void ui_box_equip_display_string(UI_Box *box, Str8 string)
{
    box->string = string;
    box->flags |= UI_Box_Flag_HasDisplayString;
}


internal UI_Key ui_key_zero(void)
{
    UI_Key key = ZERO_STRUCT;
    return key;
}

internal UI_State *ui_state_alloc(Arena *arena)
{
    UI_State *state = arena_push(arena, UI_State, 1);
    state->arena = arena;
    state->box_table_size = 4096;
    state->box_table = arena_push(state->arena, UI_Box_Slot, state->box_table_size);
    UI_InitStackNils(state);
    return state;
}

internal void ui_state_select(UI_State *state)
{
    ui_state = state;
}

internal UI_Box *ui_root_from_state(UI_State *state)
{
    return state->root;
}

internal bool ui_box_is_nil(UI_Box *box)
{
    return box == 0 || box == &ui_box_nil;
}

internal UI_Key ui_key_from_string(UI_Key seed, Str8 string)
{
    UI_Key key = ZERO_STRUCT;
    if (string.size != 0)
    {
        MemCopyStruct(&key, &seed);
        for(uint64_t i = 0; i < string.size; i += 1)
        {
            key.u64[0] = ((key.u64[0] << 5) + key.u64[0]) + string.cstr[i];
        }
    }
    return key;
}


internal Str8 ui_hash_part_from_key_string(Str8 string)
{
    uint64_t triple_pound_pos = str8_find_substr(string, 0, str8("###"), 0);
    if (triple_pound_pos < string.size)
    {
        string = str8_skip(string, triple_pound_pos);
    }
    return string;
}

internal Str8 ui_text_part_from_key_string(Str8 string)
{
    uint64_t double_pound_pos = str8_find_substr(string, 0, str8("##"), 0);
    if (double_pound_pos < string.size)
    {
        string.size = double_pound_pos;
    }
    return string;
}

internal void ui_box_equip_text(UI_Box *box, Str8 text)
{
    Unused(box); Unused(text);
    // if (box->ext_text != &ui_g_nil_box_text_ext)
    // {
    //     D_StyledStringList strings = ZERO_STRUCT;
    //     D_StyledString *sstr = D_StyledStringListPushNew(UI_BuildArena(), &strings);
    //     sstr->string = text;
    //     sstr->color = box->ext_text->text_color;
    //     sstr->font_hash = box->ext_text->font_hash;
    //     sstr->font_size = box->ext_text->font_size;
    //     box->ext_text->string = PushStr8Copy(UI_BuildArena(), text);
    //     box->ext_text->styled_runs = D_StyledRunListFromStyledStringList(UI_BuildArena(), 0, F_RasterFlag_Smooth|F_RasterFlag_Hinted, &strings);
    // }
}

internal bool ui_key_match(UI_Key a, UI_Key b)
{
    return (a.u64[0] == b.u64[0]);
}

internal UI_Box *ui_box_from_key(UI_Key key)
{
    UI_Box *result = &ui_box_nil;
    uint64_t slot = key.u64[0] % ui_state->box_table_size;
    if (!ui_key_match(key, ui_key_zero()))
    {
        for(UI_Box *b = ui_state->box_table[slot].first; !ui_box_is_nil(b); b = b->hash_next)
        {
            if (ui_key_match(b->key, key))
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
    //- ak: map the key to a box
    UI_Box *box = ui_box_from_key(key);
    //- ak: key has already been used - trample over key/box to become id-less box
    if (box->last_touched_build_index == ui_state->build_index)
    {
        box = &ui_box_nil;
        key = ui_key_zero();
    }
    //- ak: allocate the box if it has not been allocated yet, or if this is a duplicate key
    bool first_frame = 0;
    if (ui_box_is_nil(box))
    {
        uint64_t slot = key.u64[0] % ui_state->box_table_size;
        first_frame = 1;
        box = ui_state->first_free_box;
        if (ui_box_is_nil(box))
        {
            box = arena_push_nz(ui_state->arena, UI_Box, 1);
        }
        else
        {
            SLLStackPop(ui_state->first_free_box);
            MemSetZeroStruct(box);
            ui_state->free_box_list_count -= 1;
        }
        DLLInsert_NPZ(&ui_box_nil, ui_state->box_table[slot].first, ui_state->box_table[slot].last, ui_state->box_table[slot].last, box, hash_next, hash_prev);
        box->key = key;
    }
    //- ak: link it into the tree
    UI_Box *parent = ui_parent_top();
    if (ui_box_is_nil(parent))
    {
        ui_state->root = box;
    }
    else
    {
        DLLPushBack_NPZ(&ui_box_nil, parent->first, parent->last, box, next, prev);
        parent->child_count += 1;
        box->parent = parent;
    }
    //- ak: fill state
    if (!ui_box_is_nil(box))
    {
        // ak: fill per-frame state
        box->child_count = 0;
        box->first = box->last = &ui_box_nil;
        box->flags = flags|ui_flags_top();
//         box->flags |= UI_Box_Flag_FocusHot            * !!UI_IsFocusHot();
//         box->flags |= UI_Box_Flag_FocusHotDisabled    * (!UI_IsFocusHot() && ui_state->focus_hot_stack.top->v == UI_FocusKind_On);
//         box->flags |= UI_Box_Flag_FocusActive         * !!UI_IsFocusActive();
//         box->flags |= UI_Box_Flag_FocusActiveDisabled * (!UI_IsFocusActive() && ui_state->focus_active_stack.top->v == UI_FocusKind_On);
        box->pref_size[Axis_2d_X]       = ui_pref_width_top();
        box->pref_size[Axis_2d_Y]       = ui_pref_height_top();
        box->child_layout_axis          = ui_child_layout_axis_top();
        // box->hover_cursor            = UI_TopHoverCursor();
        // box->opacity                 = UI_TopOpacity();
        box->last_touched_build_index   = ui_state->build_index;

//         // ak: fill default extensions
//         box->ext_text = &ui_g_nil_box_text_ext;
//         box->ext_rect_style = &ui_g_nil_box_rect_style_ext;
//         box->ext_bucket = &ui_g_nil_box_bucket_ext;
//         box->ext_custom_draw = &ui_g_nil_box_custom_draw_ext;
//
//         // ak: equip with extensions related to feature flags
//         if (box->flags & UI_Box_Flag_DrawText)
//         {
//             box->ext_text = arena_push(UI_BuildArena(), UI_BoxTextExt, 1);
//             box->ext_text->text_alignment    = UI_TopTextAlign();
//             box->ext_text->text_edge_padding = UI_TopTextEdgePadding();
//             box->ext_text->font_hash         = UI_TopFont();
//             box->ext_text->font_size         = UI_TopFontSize();
//             box->ext_text->text_color        = UI_TopTextColor();
//         }
//         if (box->flags & (UI_Box_Flag_DrawBackground|UI_Box_Flag_DrawBorder|UI_Box_Flag_DrawOverlay))
//         {
//             box->ext_rect_style = arena_push(UI_BuildArena(), UI_BoxRectStyleExt, 1);
//             box->ext_rect_style->background_color                  = UI_TopBackgroundColor();
//             box->ext_rect_style->border_color                      = UI_TopBorderColor();
//             box->ext_rect_style->overlay_color                     = UI_TopOverlayColor();
//             box->ext_rect_style->corner_radii[Corner_00]           = UI_TopCornerRadius00();
//             box->ext_rect_style->corner_radii[Corner_01]           = UI_TopCornerRadius01();
//             box->ext_rect_style->corner_radii[Corner_10]           = UI_TopCornerRadius10();
//             box->ext_rect_style->corner_radii[Corner_11]           = UI_TopCornerRadius11();
//             box->ext_rect_style->border_thickness                  = UI_TopBorderThickness();
//         }
//
//         // ak: fill fixed positions
//         box->calc_rel_pos.x = UI_TopFixedX();
//         box->calc_rel_pos.y = UI_TopFixedY();
//
        // ak: fill first-frame state
        if (first_frame)
        {
            box->first_touched_build_index = ui_state->build_index;
        }

        // ak: is focused -> disable per stack
        // if (box->flags & UI_Box_Flag_FocusHot && !UI_IsFocusHot())
        // {
            // box->flags |= UI_Box_Flag_FocusHotDisabled;
        // }
        // if (box->flags & UI_Box_Flag_FocusActive && !UI_IsFocusActive())
        // {
            // box->flags |= UI_Box_Flag_FocusActiveDisabled;
        // }
    }

    //- ak: auto-pop
    UI_AutoPopStacks(ui_state);

    //- ak: return
    return box;
}

internal UI_Box *ui_box_make(UI_Box_Flags flags, Str8 string)
{
    //- ak: grab seed
    UI_Key seed = ui_seed_key_top();

    //- ak: produce a key from the string
    Str8 string_hash_part = ui_hash_part_from_key_string(string);
    UI_Key key = ui_key_from_string(seed, string_hash_part);

    //- ak: build the box from the key
    UI_Box *box = ui_box_make_from_key(flags, key);

    //- ak: equip box with text rendering info
    if (flags & UI_Box_Flag_DrawText)
    {
        Str8 text = ui_text_part_from_key_string(string);
        ui_box_equip_text(box, text);
    }

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
    ui_state->build_index += 1;
    ui_state->root = &ui_box_nil;
    UI_InitStacks(ui_state);
    
    // ak: prune all of the stale boxes
    for(uint64_t slot = 0; slot < ui_state->box_table_size; slot += 1)
    {
        for(UI_Box *box = ui_state->box_table[slot].first, *next = 0; !ui_box_is_nil(box); box = next)
        {
            next = box->hash_next;
            if(ui_key_match(box->key, ui_key_zero()) || box->last_touched_build_index+1 < ui_state->build_index)
            {
                
                DLLRemove_NPZ(&ui_box_nil, ui_state->box_table[slot].first, ui_state->box_table[slot].last, box, hash_next, hash_prev);
                SLLStackPush(ui_state->first_free_box, box);
                ui_state->free_box_list_count += 1;
            }
        }
    }

    // ak: zero hot key on pruned boxes
    // UI_Box *box = UI_BoxFromKey(ui_state->hot_key);
    // if(UI_BoxIsNil(box) &&
    //         (ui_key_match(ui_key_zero(), ui_state->active_key[UI_MouseButtonSlot_Left]) ||
    //          !ui_key_match(ui_state->hot_key, ui_state->active_key[UI_MouseButtonSlot_Left])) &&
    //         (ui_key_match(ui_key_zero(), ui_state->active_key[UI_MouseButtonSlot_Middle]) ||
    //          !ui_key_match(ui_state->hot_key, ui_state->active_key[UI_MouseButtonSlot_Middle])) &&
    //         (ui_key_match(ui_key_zero(), ui_state->active_key[UI_MouseButtonSlot_Right]) ||
    //          !ui_key_match(ui_state->hot_key, ui_state->active_key[UI_MouseButtonSlot_Right])))
    // {
    //     ui_state->hot_key = ui_key_zero();
    // }

    //- ak: build root
    uint32_t width = wl_window_width_get();
    uint32_t height = wl_window_height_get();
    ui_pref_width_next_set(ui_px((float)width, 1.f));
    ui_pref_height_next_set(ui_px((float)height, 1.f));
    ui_child_layout_axis_next_set(Axis_2d_Y);
    UI_Box *root = ui_box_makef(0, "window_root_###%I64x", 324);
    
    //- ak: begin root
    ui_parent_push(root);

    //- ak: push defaults
    ui_font_size_push(10.f);
    ui_text_color_push((Vec4_F32){ 1, 1, 1, 1 });
    ui_background_color_push((Vec4_F32){ 0.1f, 0.13f, 0.14f, 0.7f });
    ui_border_color_push((Vec4_F32){ 1, 1, 1, 0.2f });
    ui_pref_width_push(ui_pct(1.f, 0.f));
    ui_pref_height_push(ui_em(1.8f, 1.f));
    ui_border_size_push(1.f);
    ui_text_padding_push(ui_font_size_top() * 0.5f);
}

internal UI_Signal ui_signal_from_box(UI_Box *box)
{
    UI_Signal signal = {box};
    // UI_EventList *events = UI_Events();
    //
    // //- ak: determine clipped rectangle for this box
    // Vec2_F32 clipped_rect = box->rect;
    // for(UI_Box *b = box->parent; !ui_box_is_nil(b); b = b->parent)
    // {
    //     if (b->flags & UI_Box_Flag_Clip)
    //     {
    //         clipped_rect = Intersection2F32(clipped_rect, b->rect);
    //     }
    // }
    //
    // //- ak: determine if in context menu subtree
    // bool is_in_ctx_menu = 0;
    // for(UI_Box *b = box->parent; !ui_box_is_nil(b); b = b->parent)
    // {
    //     if (b == ui_state->ctx_menu_root)
    //     {
    //         is_in_ctx_menu = 1;
    //         break;
    //     }
    // }
    //
    // //- ak: pick blacklist rectangle, given top-level floating subtree info
    // Vec2_F32 blacklist_rect = ZERO_STRUCT;
    // if (!is_in_ctx_menu)
    // {
    //     blacklist_rect = ui_state->ctx_menu_root->rect;
    // }
    //
    // //- ak: take events
    // for(UI_Event_Node *n = events->first, *next = 0; n != 0; n = next)
    // {
    //     next = n->next;
    //     bool taken = 0;
    //     UI_Event *ev = &n->v;
    //     bool ev_in_box_interaction_region = (Contains2F32(clipped_rect, ev->pos_2f32) && !Contains2F32(blacklist_rect, ev->pos_2f32));
    //     bool ev_key_is_mouse = (ev->key == OS_Key_MouseLeft || ev->key == OS_Key_MouseRight || ev->key == OS_Key_MouseMiddle);
    //     UI_MouseButtonSlot ev_mb_slot = UI_MouseButtonSlotFromOSKey(ev->key);
    //
    //     //- ak: mouse clickability event consumption
    //     if (box->first_gen_touched != box->last_gen_touched && box->flags & UI_Box_Flag_MouseClickable)
    //     {
    //         if (ev_key_is_mouse && ev_in_box_interaction_region && ev->kind == UI_EventKind_Press)
    //         {
    //             taken = 1;
    //             ui_state->hot_key = ui_state->active_key[ev_mb_slot] = box->key;
    //             signal.flags |= UI_Signal_Flag_PressedLeft<<ev_mb_slot;
    //             ui_state->drag_start_mouse = ev->pos_2f32;
    //         }
    //         if (ev_key_is_mouse && ev->kind == UI_EventKind_Release && ui_key_match(ui_state->active_key[ev_mb_slot], box->key))
    //         {
    //             taken = 1;
    //             signal.flags |= UI_Signal_Flag_ReleasedLeft<<ev_mb_slot;
    //             if (ev_in_box_interaction_region)
    //             {
    //                 signal.flags |= UI_Signal_Flag_ClickedLeft<<ev_mb_slot;
    //             }
    //             ui_state->active_key[ev_mb_slot] = ui_key_zero();
    //         }
    //     }
    //
    //     //- ak: keyboard clickability event consumption
    //     if (box->flags & UI_Box_Flag_KeyboardClickable && box->flags & UI_Box_Flag_FocusHot && !(box->flags & UI_Box_Flag_FocusHotDisabled) &&
    //             ev->kind == UI_EventKind_Press && ev->ctrl_slot == UI_CtrlSlot_Accept)
    //     {
    //         taken = 1;
    //         signal.flags |= UI_Signal_Flag_ClickedLeft|UI_Signal_Flag_PressedLeft|UI_Signal_Flag_PressedKeyboard;
    //     }
    //
    //     //- ak: scrolling event consumption
    //     if (box->flags & UI_Box_Flag_ViewScroll && ev->kind == UI_EventKind_Scroll && ev_in_box_interaction_region)
    //     {
    //         taken = 1;
    //         for EachEnumVal(axis, Axis_2d)
    //         {
    //             box->target_view_off.v[axis] += ev->delta_2f32.v[axis];
    //             if (box->flags & (UI_Box_Flag_OverflowX<<axis))
    //             {
    //                 UI_LayoutRoot(box, axis);
    //             }
    //             Rng1F32 scroll_bounds = UI_ScrollBoundsFromBox(box, axis);
    //             box->target_view_off.v[axis] = Clamp1F32(scroll_bounds, box->target_view_off.v[axis]);
    //         }
    //     }
    //
    //     //- ak: consume
    //     if (taken)
    //     {
    //         UI_EatEvent(events, ev);
    //     }
    // }
    //
    // //- ak: fill out flags & state based on polled information
    // Vec2_F32 mouse_polled = UI_Mouse();
    // if (Contains2F32(clipped_rect, mouse_polled))
    // {
    //     signal.flags |= UI_Signal_Flag_MouseIsOver;
    // }
    // if (Contains2F32(clipped_rect, mouse_polled) && !Contains2F32(blacklist_rect, mouse_polled))
    // {
    //     if (ui_key_match(ui_key_zero(), ui_state->hot_key))
    //     {
    //         signal.flags |= UI_Signal_Flag_Hovering;
    //         bool is_any_key_active = 0;
    //         for EachEnumVal(slot, UI_MouseButtonSlot)
    //         {
    //             if (!ui_key_match(ui_state->active_key[slot], ui_key_zero()))
    //             {
    //                 is_any_key_active = 1;
    //                 break;
    //             }
    //         }
    //         if (!is_any_key_active)
    //         {
    //             ui_state->hot_key = box->key;
    //         }
    //     }
    // }
    // else if (ui_key_match(box->key, ui_state->hot_key))
    // {
    //     ui_state->hot_key = ui_key_zero();
    // }
    // if (box->flags & UI_Box_Flag_MouseClickable)
    // {
    //     for EachEnumVal(slot, UI_MouseButtonSlot)
    //     {
    //         if (ui_key_match(ui_state->active_key[slot], box->key))
    //         {
    //             signal.flags |= UI_Signal_Flag_DraggingLeft<<slot;
    //             ui_state->hot_key = box->key;
    //         }
    //     }
    // }

    return signal;
}

internal UI_Box_Step ui_box_step_next(UI_Box *box, UI_Box *root, uint64_t sib_member_off, uint64_t child_member_off)
{
    UI_Box_Step result = ZERO_STRUCT;
    result.next = &ui_box_nil;
    if(!ui_box_is_nil(*MemFromOffset(UI_Box **, box, child_member_off)))
    {
        result.next = *MemFromOffset(UI_Box **, box, child_member_off);
        result.push_count = 1;
    }
    else for(UI_Box *p = box; !ui_box_is_nil(p) && p != root; p = p->parent)
    {
        if(!ui_box_is_nil(*MemFromOffset(UI_Box **, p, sib_member_off)))
        {
            result.next = *MemFromOffset(UI_Box **, p, sib_member_off);
            break;
        }
        result.pop_count += 1;
    }
    return result;
}

internal void ui_pref_size_push(Axis_2d axis, UI_Size v)
{
    (axis == Axis_2d_X ? ui_pref_width_push : ui_pref_height_push)(v);
}

internal void ui_pref_size_pop(Axis_2d axis)
{
    (axis == Axis_2d_X ? ui_pref_width_pop : ui_pref_height_pop)();
}

internal void ui_pref_size_next_set(Axis_2d axis, UI_Size v)
{
    (axis == Axis_2d_X ? ui_pref_width_next_set : ui_pref_height_next_set)(v);
}
