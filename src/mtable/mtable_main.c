//~ ak: Includes
//=============================================================================

//- ak: headers
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "./mtable.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "./mtable.c"

void base_main(void)
{
    Str8 src_path = str8("./src");
    Arena *arena = arena_alloc(MB(10), MB(1));
    //- ak: collect file paths ================================================
    Str8List file_paths = {0};
    fmt_printf("Searching %.*s...", str8_varg(src_path));
    {
        typedef struct Dir Dir;
        struct Dir
        {
            Dir *next;
            Str8 src_path;
        };
        Dir start_dir = {0, src_path};
        Dir *first_dir = &start_dir;
        Dir *last_dir = &start_dir;
        for(Dir *dir = first_dir; dir != 0; dir = dir->next)
        {
            Os_File_Walk *walk = os_file_walk_begin(arena, dir->src_path, 0);
            for(Os_File_Info info = ZERO_STRUCT; os_file_walk_next(arena, walk, &info);)
            {
                Str8 file_path = str8f(arena, "%.*s/%.*s", str8_varg(dir->src_path), str8_varg(info.name));
                if(info.props.flags & Os_File_Property_Flag_IsFolder)
                {
                    Dir *next_dir = arena_push(arena, Dir, 1);
                    SLLQueuePush(first_dir, last_dir, next_dir);
                    next_dir->src_path = file_path;
                }
                else
                {
                    str8_list_push(arena, &file_paths, file_path);
                }
            }
            os_file_walk_end(walk);
        }
    }
    fmt_printfln(" %i directory found", (int)file_paths.length);
    //- ak: parse all metatable files =========================================
    fmt_printf("parsing metatable...");
    {
        for(Str8Node *n = file_paths.first; n != 0; n = n->next)
        {
            Str8 file_path = n->str;
            Str8 file_ext = str8_skip_last_dot(file_path);
            if(str8_match(file_ext, str8("mtable"), 0))
            {
                Str8 source = os_path_read_str_full(file_path, arena);
                MT_Tokenize tokenize = mt_tokenize_from_str8(source, arena);
                // MT_Parse parse = mt_parse_from_tokens(tokenize.tokens, data, file_path, arena);
            }
        }
    }
    // printf(" %i metable files parsed\n", (int)parses.count);
}
