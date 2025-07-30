#ifndef BASE_CMD_H
#define BASE_CMD_H

// Command Line Types
//=============================================================================

typedef struct CmdLineOpt CmdLineOpt;
struct CmdLineOpt
{
    CmdLineOpt *next;
    CmdLineOpt *hash_next;
    U64 hash;
    Str8 string;
    Str8List value_strings;
    Str8 value_string;
};

typedef struct CmdLineOptList CmdLineOptList;
struct CmdLineOptList
{
  U64 count;
  CmdLineOpt *first;
  CmdLineOpt *last;
};

typedef struct CmdLine CmdLine;
struct CmdLine
{
    Str8 exe_name;
    CmdLineOptList options;
    Str8List inputs;
    U64 option_table_size;
    CmdLineOpt **option_table;
    Str8List args_list;
};

// Command Line Parsing Functions
//=============================================================================

internal CmdLineOpt  *cmd_opt_from_slot(CmdLineOpt **slot, Str8 string);
internal CmdLineOpt  **cmd_slot_from_string(CmdLine *cmd_line, Str8 string);
internal CmdLine     cmd_parsed(Alloc alloc, Str8List args_list);
internal CmdLineOpt  *cmd_opt_from_string(CmdLine *cmd_line, Str8 name);
internal Str8List    cmd_strings(CmdLine *cmd_line, Str8 name);
internal Str8        cmd_string(CmdLine *cmd_line, Str8 name);
internal bool        cmd_has_flag(CmdLine *cmd_line, Str8 name);
internal bool        cmd_has_argument(CmdLine *cmd_line, Str8 name);

#endif // BASE_CMD_H
