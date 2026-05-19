internal UI_State *ui_state_alloc(Arena *arena)
{
    UI_State *state = arena_push(arena, UI_State, 1);
    state->arena = arena;
    UI_InitStackNils(state);
    return state;
}
internal UI_Box *ui_root_from_state(UI_State *state)
{
    return state->root;
}

internal bool ui_box_is_nil(UI_Box *box)
{
    return box == 0 || box == &ui_box_nil;
}

internal UI_Box *ui_box_make(UI_Box_Flags flags, Str8 string)
{
    TempUnused(string);
    UI_Box *box = arena_push(ui_state->arena, UI_Box, 1);
    box->flags = flags;
    
    //- ak: link it into the tree
    UI_Box *parent = ui_parent_top();
    if (ui_box_is_nil(parent))
    {
        ui_state->root = box;
    }
    else
    {
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

internal UI_Box_Rec ui_box_rec_df(UI_Box *box, UI_Box *root, uint64_t sib_member_off, uint64_t child_member_off)
{
    UI_Box_Rec result = ZERO_STRUCT;
    if(!ui_box_is_nil(*MemFromOffset(UI_Box **, box, child_member_off)))
    {
        result.next = *MemFromOffset(UI_Box **, box, child_member_off);
        result.push_count = 1;
    }
    else for (UI_Box *p = box; !ui_box_is_nil(p) && p != root; p = p->parent)
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

internal void _ui_calc_sizes_standalone(UI_Box *root, Axis_2d axis)
{
    for (UI_Box *b = root; !ui_box_is_nil(b); b = ui_box_rec_df_pre(b, root).next)
    {
        switch(b->pref_size[axis].kind)
        {
            default:{}break;
            case UI_Size_Kind_Pixels:
            {
                b->fixed_size.v[axis] = b->pref_size[axis].value;
            }break;
            case UI_Size_Kind_TextContent:
            {
                // float padding = b->pref_size[axis].value;
                // float text_size = b->display_fruns.dim.x;
                // b->fixed_size.v[axis] = padding + text_size + b->text_padding*2;
            }break;
        }
    }
}

internal void _ui_calc_sizes_upwards_dependent(UI_Box *root, Axis_2d axis)
{
    for (UI_Box *b = root; !ui_box_is_nil(b); b = ui_box_rec_df_pre(b, root).next)
    {
        switch(b->pref_size[axis].kind)
        {
            default:{}break;
            case UI_Size_Kind_ParentPct:
            {
                // rjf: find parent that has a fixed size
                UI_Box *fixed_parent = &ui_box_nil;
                for (UI_Box *p = b->parent; !ui_box_is_nil(p); p = p->parent)
                {
                    if(p->flags & (UI_Box_Flag_FixedWidth<<axis) ||
                            p->pref_size[axis].kind == UI_Size_Kind_Pixels ||
                            p->pref_size[axis].kind == UI_Size_Kind_TextContent ||
                            p->pref_size[axis].kind == UI_Size_Kind_ParentPct)
                    {
                        fixed_parent = p;
                        break;
                    }
                }

                // rjf: figure out box's size on this axis
                float size = fixed_parent->fixed_size.v[axis] * b->pref_size[axis].value;

                // rjf: mutate box to have this size
                b->fixed_size.v[axis] = size;
            }break;
        }
    }
}

internal void _ui_calc_sizes_downwards_dependent(UI_Box *root, Axis_2d axis)
{
  UI_Box_Rec rec = {0};
  for (UI_Box *box = root; !ui_box_is_nil(box); box = rec.next)
  {
    rec = ui_box_rec_df_pre(box, root);
    size_t pop_idx = 0;
    for (UI_Box *b = box;
        !ui_box_is_nil(b) && pop_idx <= rec.pop_count;
        b = b->parent, pop_idx += 1)
    {
      if(b->pref_size[axis].kind == UI_Size_Kind_ChildrenSum)
      {
        float sum = 0;
        for (UI_Box *child = b->first_child;
            !ui_box_is_nil(child);
            child = child->next_sibling)
        {
          if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
          {
            if(axis == b->child_layout_axis)
            {
              sum += child->fixed_size.v[axis];
            }
            else
            {
              sum = Max(sum, child->fixed_size.v[axis]);
            }
          }
        }
        b->fixed_size.v[axis] = sum;
      }
    }
  }
}


internal void _ui_layout_enforce_constraints(UI_Box *root, Axis_2d axis)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    for (UI_Box *box = root; !ui_box_is_nil(box); box = ui_box_rec_df_pre(box, root).next)
    {
        //- rjf: fixup children sizes (if we're solving along the *non-layout* axis)
        if(axis != box->child_layout_axis
            && !(box->flags & (UI_Box_Flag_AllowOverflowX << axis)))
        {
            float allowed_size = box->fixed_size.v[axis];
            for (UI_Box *child = box->first_child;
                !ui_box_is_nil(child);
                child = child->next_sibling)
            {
                if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
                {
                    float child_size = child->fixed_size.v[axis];
                    float violation = child_size - allowed_size;
                    float max_fixup = child_size;
                    float fixup = Clamp(0, violation, max_fixup);
                    if(fixup > 0)
                    {
                        child->fixed_size.v[axis] -= fixup;
                    }
                }
            }
        }

        //- rjf: fixup children sizes (in the direction of the layout axis)
        if(axis == box->child_layout_axis && !(box->flags & (UI_Box_Flag_AllowOverflowX << axis)))
        {
            // rjf: figure out total allowed size & total size
            float total_allowed_size = box->fixed_size.v[axis];
            float total_size = 0;
            float total_weighted_size = 0;
            for (UI_Box *child = box->first_child;
                !ui_box_is_nil(child);
                child = child->next_sibling)
            {
                if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
                {
                    total_size += child->fixed_size.v[axis];
                    total_weighted_size += child->fixed_size.v[axis] * (1-child->pref_size[axis].strictness);
                }
            }

            // rjf: if we have a violation, we need to subtract some amount from all children
            float violation = total_size - total_allowed_size;
            if(violation > 0 && total_weighted_size > 0)
            {
                Arena_Temp temp = arena_temp_begin(scratch.arena);

                // rjf: figure out how much we can take in totality
                float child_fixup_sum = 0;
                float *child_fixups = arena_push(temp.arena, float, box->child_count);
                {
                    uint64_t child_idx = 0;
                    for (UI_Box *child = box->first_child;
                        !ui_box_is_nil(child);
                        child = child->next_sibling, child_idx += 1)
                    {
                        if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
                        {
                            float fixup_size_this_child = child->fixed_size.v[axis] * (1-child->pref_size[axis].strictness);
                            fixup_size_this_child = Max(0, fixup_size_this_child);
                            child_fixups[child_idx] = fixup_size_this_child;
                            child_fixup_sum += fixup_size_this_child;
                        }
                    }
                }

                // rjf: fixup child sizes
                {
                    uint64_t child_idx = 0;
                    for (UI_Box *child = box->first_child; !ui_box_is_nil(child); child = child->next_sibling, child_idx += 1)
                    {
                        if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
                        {
                            float fixup_pct = (violation / total_weighted_size);
                            fixup_pct = Clamp(0, fixup_pct, 1);
                            child->fixed_size.v[axis] -= child_fixups[child_idx] * fixup_pct;
                        }
                    }
                }

                arena_temp_end(temp);
            }
        }

        //- rjf: fixup upwards-relative sizes
        if(box->flags & (UI_Box_Flag_AllowOverflowX << axis))
        {
            for (UI_Box *child = box->first_child; !ui_box_is_nil(child); child = child->next_sibling)
            {
                if(child->pref_size[axis].kind == UI_Size_Kind_ParentPct)
                {
                    child->fixed_size.v[axis] = box->fixed_size.v[axis] * child->pref_size[axis].value;
                }
            }
        }

        //- rjf: enforce clamps
        for (UI_Box *child = box->first_child; !ui_box_is_nil(child); child = child->next_sibling)
        {
            child->fixed_size.v[axis] = Max(child->fixed_size.v[axis], child->min_size.v[axis]);
        }
    }
    arena_scratch_end(scratch);
}

internal void _ui_layout_position(UI_Box *root, Axis_2d axis)
{
    for (UI_Box *box = root; !ui_box_is_nil(box); box = ui_box_rec_df_pre(box, root).next)
    {
        float layout_position = 0;

        //- rjf: lay out children
        float bounds = 0;
        for (UI_Box *child = box->first_child; !ui_box_is_nil(child); child = child->next_sibling)
        {
            // rjf: grab original position
            float original_position = Min(child->rect.p0.v[axis], child->rect.p1.v[axis]);

            // rjf: calculate fixed position & size
            if(!(child->flags & (UI_Box_Flag_FloatingX<<axis)))
            {
                child->fixed_position.v[axis] = layout_position;
                if(box->child_layout_axis == axis)
                {
                    layout_position += child->fixed_size.v[axis];
                    bounds += child->fixed_size.v[axis];
                }
                else
                {
                    bounds = Max(bounds, child->fixed_size.v[axis]);
                }
            }

            // rjf: determine final rect for child, given fixed_position & size
            // if(child->flags & (UI_Box_Flag_AnimatePosX<<axis))
            // {
            //     if(child->first_touched_build_index == child->last_touched_build_index)
            //     {
            //         child->fixed_position_animated = child->fixed_position;
            //     }
            //     child->rect.p0.v[axis] = box->rect.p0.v[axis] + child->fixed_position_animated.v[axis] - !(child->flags&(UI_Box_Flag_SkipViewOffX<<axis))*floor_f32(box->view_off.v[axis]);
            // }
            // else
            // {
                child->rect.p0.v[axis] = box->rect.p0.v[axis] + child->fixed_position.v[axis] - !(child->flags&(UI_Box_Flag_SkipViewOffX<<axis))*floor_f32(box->view_off.v[axis]);
            // }
            child->rect.p1.v[axis] = child->rect.p0.v[axis] + child->fixed_size.v[axis];
            child->rect.p0.x = floor_f32(child->rect.p0.x);
            child->rect.p0.y = floor_f32(child->rect.p0.y);
            child->rect.p1.x = floor_f32(child->rect.p1.x);
            child->rect.p1.y = floor_f32(child->rect.p1.y);

            // rjf: grab new position
            float new_position = Min(child->rect.p0.v[axis], child->rect.p1.v[axis]);

            // rjf: store position delta
            child->position_delta.v[axis] = new_position - original_position;
        }

        //- rjf: store view bounds
        {
            box->view_bounds.v[axis] = bounds;
        }
    }
}

internal void ui_layout(UI_Box *root, Axis_2d axis)
{
  _ui_calc_sizes_standalone(root, axis);
  _ui_calc_sizes_upwards_dependent(root, axis);
  _ui_calc_sizes_downwards_dependent(root, axis);
  _ui_layout_enforce_constraints(root, axis);
  _ui_layout_position(root, axis);
}
internal void ui_build_begin(UI_State *state)
{
    ui_state = state;
    ui_state->root = &ui_box_nil;
    UI_InitStacks(ui_state);
    
    // ak: build root
    uint32_t width = wl_window_width_get();
    uint32_t height = wl_window_height_get();
    ui_pref_width_next_set(ui_px((float)width, 1.f));
    ui_pref_height_next_set(ui_px((float)height, 1.f));
    ui_child_axis_next_set(Axis_2d_Y);
    UI_Box *root = ui_box_makef(UI_Box_Flag_DrawBackground, "window_root_###%I64x", 324);
    ui_parent_push(root);
}

internal void ui_build_end(void)
{
    for (Axis_2d axis = (Axis_2d)0; axis < Axis_2d_COUNT; axis = (Axis_2d)(axis + 1))
    {
      ui_layout(ui_state->root, axis);
    }
}
