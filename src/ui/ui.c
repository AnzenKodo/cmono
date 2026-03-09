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
    return old_value;

#define Ui_StackPop(state, name_upper, name_lower) \
    Ui_##name_upper##_Node *popped = state->name_lower##_stack.top;\
    if(popped != &state->name_lower##_nil_stack_top)\
    {\
        SLLStackPop(state->name_lower##_stack.top);\
        SLLStackPush(state->name_lower##_stack.free, popped);\
        state->name_lower##_stack.auto_pop = false;\
        state->name_lower##_stack.gen += 1;\
    }\
    return popped->v;\

//~ ak: Core functions
//=============================================================================

typedef enum Axies
{
    Axies_X,
    Axies_Y,
    Axies_COUNT,
}
Axies;

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

struct Ui_Size
{
    Ui_Size_Kind kind;
    float value;
    float strictness;
};

typedef struct Ui_Box Ui_Box;
struct Ui_Box
{
    Ui_Box *hash_next;
    Ui_Box *hash_prev;
    Ui_Box *first;
    Ui_Box *last;
    Ui_Box *next;
    Ui_Box *prev;
    Ui_Box *parent;
    size_t child_count;
};

read_only global Ui_Box ui_box_nil =
{
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    0,
};

internal Ui_Box ui_box()
{
    Ui_Box box = ZERO_STRUCT;
    return box;
}



typedef struct Ui_Parent_Node Ui_Parent_Node;
struct Ui_Parent_Node
{
    Ui_Parent_Node *next;
    Ui_Box *v;
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

// internal void ui_font_push(Font *font)
// {
// }
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

