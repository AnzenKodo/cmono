// NOTE(ak): This UI module code is reference from:
// - https://www.dgtlgrove.com/p/ui-part-2-build-it-every-frame-immediate
// - https://www.dgtlgrove.com/p/ui-part-3-the-widget-building-language
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.h
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.c

#ifndef UI_CORE_H
#define UI_CORE_H

typedef struct UI_Key UI_Key;
struct UI_Key
{
    uint64_t u64[1];
};

typedef unsigned int UI_Box_Flags;
enum
{
    UI_Box_Flag_DrawBackground    = (1<<0),
};

typedef struct UI_Box UI_Box;
struct UI_Box
{
    UI_Box *hash_next;
    UI_Box_Flags flags;
    UI_Key key;
};

typedef struct UI_Box_Hash_Slot UI_Box_Hash_Slot;
struct UI_Box_Hash_Slot
{
    UI_Box *hash_first;
    UI_Box *hash_last;
};

typedef struct UI_State UI_State;
struct UI_State
{
    Arena *arena;
    UI_Box_Hash_Slot *box_table;
    size_t box_table_size;
};


PragmaNoWarnMissingFieldInitPush()

read_only global UI_Box ui_box_nil =
{
    &ui_box_nil,
};

PragmaPop()

global UI_State *ui_state = 0;

#endif // UI_CORE_H
