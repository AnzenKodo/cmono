// NOTE(aman.v): the lib inspiration taken from:
//  - [Ryan Fleury](https://www.dgtlgrove.com)
//      - [Table-Driven Code Generation](https://www.dgtlgrove.com/p/table-driven-code-generation)
//      - [Metadesk](https://github.com/ryanfleury/metadesk)
//      - [RadDebugger Metadesk](https://github.com/EpicGamesExt/raddebugger/tree/master/src/mdesk)

#ifndef MTABLE_H
#define MTABLE_H

//~ ak: Types
//=============================================================================

typedef uint32_t MT_Token_Flags;
enum
{
    // ak: base kind info
    MT_Token_Flag_Identifier          = (1<<0),
    MT_Token_Flag_Numeric             = (1<<1),
    MT_Token_Flag_StringLiteral       = (1<<2),
    MT_Token_Flag_Symbol              = (1<<3),
    MT_Token_Flag_Reserved            = (1<<4),
    MT_Token_Flag_Comment             = (1<<5),
    MT_Token_Flag_Whitespace          = (1<<6),
    MT_Token_Flag_Newline             = (1<<7),
    // ak: decoration info
    MT_Token_Flag_StringSingleQuote   = (1<<8),
    MT_Token_Flag_StringDoubleQuote   = (1<<9),
    // ak: error info
    MT_Token_Flag_BrokenComment       = (1<<10),
    MT_Token_Flag_BrokenStringLiteral = (1<<11),
    MT_Token_Flag_BadCharacter        = (1<<12),
};

typedef struct MT_Token MT_Token;
struct MT_Token
{
    Rng1_U64 range;
    MT_Token_Flags flags;
};

typedef struct MT_Token_Array MT_Token_Array;
struct MT_Token_Array
{
    MT_Token *v;
    uint64_t count;
};


typedef struct MT_Token_Batch_Node MT_Token_Batch_Node;
struct MT_Token_Batch_Node
{
    MT_Token_Batch_Node *next;
    MT_Token *v;
    size_t count;
    size_t cap;
};

typedef struct MT_Token_Batch_List MT_Token_Batch_List;
struct MT_Token_Batch_List
{
    MT_Token_Batch_Node *first;
    MT_Token_Batch_Node *last;
    size_t chunk_count;
    size_t total_token_count;
};

typedef struct MT_Tokenize MT_Tokenize;
struct MT_Tokenize
{
    MT_Token_Array tokens;
    // MT_Msg_List msgs;
};

//~ ak: Functions
//=============================================================================

internal MT_Tokenize mt_tokenize_from_str8(Str8 text, Arena *arena);

#endif // MTABLE_H
