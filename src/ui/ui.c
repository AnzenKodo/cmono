#define UI_StackTopImpl(state, name_upper, name_lower) \
    return state->name_lower##_stack.top->v

#define UI_StackBottomImpl(state, name_upper, name_lower) \
    return state->name_lower##_stack.bottom_val

#define Ui_StackPush(state, name_upper, name_lower, type, new_value) \
    Ui_##name_upper##_Node *node = state->name_lower##_stack.free;\
    if(node != 0) {SLLStackPop(state->name_lower##_stack.free);}\
    else {node = arena_push(state->arena, Ui_##name_upper##_Node, 1);}\
    type old_value = state->name_lower##_stack.top->v;\
    node->v = new_value;\
    SLLStackPush(state->name_lower##_stack.top, node);\
    if(node->next == &state->name_lower##_nil_stack_top)\
    {\
        state->name_lower##_stack.bottom_val = (node->v);\
    }\
    state->name_lower##_stack.auto_pop = false;\
    state->name_lower##_stack.gen += 1;\
    return old_value

#define Ui_StackPop(state, name_upper, name_lower) \
    Ui_##name_upper##_Node *popped = state->name_lower##_stack.top;\
    if(popped != &state->name_lower##_nil_stack_top)\
    {\
        SLLStackPop(state->name_lower##_stack.top);\
        SLLStackPush(state->name_lower##_stack.free, popped);\
        state->name_lower##_stack.auto_pop = false;\
        state->name_lower##_stack.gen += 1;\
    }\
    return popped->v

//~ ak: Core functions
//=============================================================================

typedef enum Axis_2d
{
    Axis_2d_X,
    Axis_2d_Y,
    Axis_2d_COUNT,
}
Axis_2d;

typedef enum Axis_3d
{
    Axis_3d_X,
    Axis_3d_Y,
    Axis_3d_Z,
    Axis_3d_COUNT,
}
Axis_3d;

typedef unsigned int Ui_Box_Flags;
enum
{
    Ui_Box_Flag_ScrollX = (1<<0),
    Ui_Box_Flag_ScrollY = (1<<1),
};

typedef enum Ui_Size_Kind
{
  Ui_Size_Kind_Pixels,
} Ui_Size_Kind;

typedef struct Ui_Size Ui_Size;
struct Ui_Size
{
    Ui_Size_Kind kind;
    float value;
    float strictness;
};

#define ui_px(value, strictness)         (Ui_Size){ UI_SizeKind_Pixels, value, strictness }
#define ui_em(value, strictness)         (Ui_Size){ UI_SizeKind_Pixels, (value) * ui_top_font_size(), strictness }
#define ui_text_dim(padding, strictness) (Ui_Size){ UI_SizeKind_TextContent, padding, strictness }
#define ui_pct(value, strictness)        (Ui_Size){ UI_SizeKind_ParentPct, value, strictness }
#define ui_children_sum(strictness)      (Ui_Size){ UI_SizeKind_ChildrenSum, 0.f, strictness }

typedef struct Ui_Box Ui_Box;
struct Ui_Box
{
    //- ak: persistent links
    Ui_Box *hash_next;
    Ui_Box *hash_prev;
    //- ak: per-build links/data
    Ui_Box *first;
    Ui_Box *last;
    Ui_Box *next;
    Ui_Box *prev;
    Ui_Box *parent;
    size_t child_count;
    //- rjf: per-build equipment
    // UI_Key key;
    Ui_Box_Flags flags;
    // UI_Key tags_key;
    Str8 string;
// UI_TextAlign text_align;
    // Vec2F32 fixed_position;
    // Vec2F32 fixed_size;
    // Vec2F32 min_size;
    Ui_Size pref_size[Axis_2d_COUNT];
    Axis_2d child_layout_axis;
    // OS_Cursor hover_cursor;
    // U32 fastpath_codepoint;
    // UI_Key group_key;
    // DR_Bucket *draw_bucket;
// UI_BoxCustomDrawFunctionType *custom_draw;
    // void *custom_draw_user_data;
    Vec4_F32 background_color;
    Vec4_F32 text_color;
    Vec4_F32 border_color;
    Font *font;
    float font_size;
    // F32 tab_size;
    // FNT_RasterFlags text_raster_flags;
    // F32 corner_radii[Corner_COUNT];
    // F32 blur_size;
    // F32 transparency;
    // F32 squish;
    float text_padding;
  //- rjf: per-build artifacts
  // DR_FStrList display_fstrs;
  // DR_FRunList display_fruns;
    Vec2_F32 rect;
  // Vec2F32 fixed_position_animated;
  // Vec2F32 position_delta;
  // FuzzyMatchRangeList fuzzy_match_ranges;
  //
  // //- rjf: persistent data
  // U64 first_touched_build_index;
  // U64 last_touched_build_index;
  // U64 first_disabled_build_index;
  // F32 hot_t;
  // F32 active_t;
  // F32 disabled_t;
  // F32 focus_hot_t;
  // F32 focus_active_t;
  // F32 focus_active_disabled_t;
  // Vec2F32 view_off;
  // Vec2F32 view_off_target;
  // Vec2F32 view_bounds;
  // UI_Key default_nav_focus_hot_key;
  // UI_Key default_nav_focus_active_key;
  // UI_Key default_nav_focus_next_hot_key;
  // UI_Key default_nav_focus_next_active_key;
};

PragmaNoWarnMissingFieldInitPush()

read_only global Ui_Box ui_box_nil =
{
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
};

PragmaPop()

internal Ui_Box ui_box()
{
    Ui_Box box = ZERO_STRUCT;
    // box.font =
    return box;
}



typedef struct Ui_Parent_Node Ui_Parent_Node;
struct Ui_Parent_Node
{
    Ui_Parent_Node *next;
    Ui_Box *v;
};
typedef struct Ui_Font_Node Ui_Font_Node;
struct Ui_Font_Node
{
    Ui_Font_Node *next;
    Font *v;
};

typedef struct Ui_State Ui_State;
struct Ui_State
{
    Arena *arena;
    struct
    {
        Ui_Parent_Node *top;
        Ui_Box *bottom_val;
        Ui_Parent_Node *free;
        bool auto_pop;
        size_t gen;
    } parent_stack;
    Ui_Parent_Node parent_nil_stack_top;
    struct
    {
        Ui_Font_Node *top;
        Font *bottom_val;
        Ui_Font_Node *free;
        bool auto_pop;
        size_t gen;
    } font_stack;
    Ui_Font_Node font_nil_stack_top;
};

global Ui_State *ui_state = 0;

internal Ui_State *ui_state_alloc(Arena *arena)
{
    Ui_State *state = arena_push(arena, Ui_State, 1);
    state->arena = arena;
    state->parent_nil_stack_top.v = &ui_box_nil;
    state->parent_stack.top = &state->parent_nil_stack_top;
    state->parent_stack.bottom_val = &ui_box_nil;
    state->parent_stack.free = 0;
    state->parent_stack.auto_pop = 0;
    state->font_nil_stack_top.v = 0;
    state->font_stack.top = &state->font_nil_stack_top;
    state->font_stack.bottom_val = 0;
    state->font_stack.free = 0;
    state->font_stack.auto_pop = 0;
    return state;
}

internal void ui_state_select(Ui_State *state)
{
    ui_state = state;
}

internal Ui_Box *ui_parent_push(Ui_Box *box)
{
    Ui_StackPush(ui_state, Parent, parent, Ui_Box *, box);
}
internal Ui_Box *ui_parent_pop(void)
{
    Ui_StackPop(ui_state, Parent, parent);
}

internal Font *ui_font_push(Font *font_val)
{
    Ui_StackPush(ui_state, Font, font, Font *, font_val);
}
//
// internal void ui_font_pop(Font *font)
// {
// }
//
// internal void ui_box_flags_push(Ui_Box_Flags flags)
// {
// }
//
// internal void ui_box_flags_pop(Ui_Box_Flags flags)
// {
// }

//~ ak: Basic widgets functions
//=============================================================================

