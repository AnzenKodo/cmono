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

typedef struct Ui_State Ui_State;
struct Ui_State {
    struct {
        UI_FlagsNode *top;
        UI_BoxFlags bottom_val;
        UI_FlagsNode *free;
    } flags_stack;
    Font font;
};

internal void ui_box(void)
{
}

internal void ui_font_push(Font *font)
{
}

internal void ui_font_pop(Font *font)
{
}

internal void ui_box_flags_push(Ui_Box_Flags flags)
{
}

internal void ui_box_flags_pop(Ui_Box_Flags flags)
{
}

//~ ak: Basic widgets functions
//=============================================================================

