//~ ak: Includes
//=============================================================================

//- ak: headers
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "./mdesk.h"
#include "./mdesk_gen.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "./mdesk.c"
#include "./mdesk_gen.c"

void base_main(void)
{
    Str8 src_path = str8("src");
    Str8 defulat_gen_dirname = str8("generated");
    Str8 ext_name = str8("mdesk");
    MDG_Msg_List msgs = ZERO_STRUCT;
    Arena *arena = arena_alloc(MB(10), MB(1));
    MDG_State *state = mdg_state_init(256, arena);
    
    //- ak: collect file paths ================================================
    Str8_List file_paths = ZERO_STRUCT;
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
        for (Dir *dir = first_dir; dir != 0; dir = dir->next)
        {
            Os_File_Walk *walk = os_file_walk_begin(arena, dir->src_path, 0);
            for (Os_File_Info info = ZERO_STRUCT; os_file_walk_next(arena, walk, &info);)
            {
                Str8 file_path = str8f(arena, "%.*s/%.*s", str8_varg(dir->src_path), str8_varg(info.name));
                if (info.props.flags & Os_File_Property_Flag_IsFolder)
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
    fmt_printfln(" %zu directory found", file_paths.length);
    
    //- ak: parse all metatable files =========================================
    fmt_printf("parsing metatable...");
    MDG_ParsedFile_List parses = ZERO_STRUCT;
    {
        for (Str8_Node *n = file_paths.first; n != 0; n = n->next)
        {
            Str8 file_path = n->str;
            Str8 file_ext = str8_skip_last_dot(file_path);
            if (str8_match(file_ext, ext_name, 0))
            {
                Str8 source = os_path_read_str_full(file_path, arena);
                MD_Parse parse = md_parse_from_string(source, file_path, arena);
                for (MD_Msg *m = parse.msgs.first; m != 0; m = m->next)
                {
                    Log_Level msg_level = Log_Level_None;
                    switch (m->level)
                    {
                        case MD_Msg_Level_None:  {}break;
                        case MD_Msg_Level_Info:  {msg_level = Log_Level_Info;}break;
                        case MD_Msg_Level_Debug: {msg_level = Log_Level_Debug;}break;
                        case MD_Msg_Level_Warn:  {msg_level = Log_Level_Warn;}break;
                        case MD_Msg_Level_Error: {msg_level = Log_Level_Error;}break;
                    }
                    Txt_Pt pt = str8_offset_to_txt_pt(source, m->node->src_offset);
                    MDG_Msg dst_m = {msg_level, pt, file_path, m->string};
                    mdg_msg_list_push(&msgs, &dst_m, arena);
                }
                MDG_ParsedFile_Node *parse_n = arena_push(arena, MDG_ParsedFile_Node, 1);
                SLLQueuePush(parses.first, parses.last, parse_n);
                parse_n->v.root = parse.root;
                parses.count += 1;
            }
        }
    }
    fmt_printfln(" %zu .%.*s files parsed", parses.count, str8_varg(ext_name));
    
    //- ak: gather tables =====================================================
    MDG_Map table_grid_map = mdg_map_push(1024, arena);
    MDG_Map table_col_map = mdg_map_push(1024, arena);
    size_t table_count = 0;
    fmt_printf("gathering tables...");
    {
        for (MDG_ParsedFile_Node *n = parses.first; n != 0; n = n->next)
        {
            MD_Node *file = n->v.root;
            for (MD_Node *node = file->first; !md_node_is_nil(node); node = node->next)
            {
                MD_Node *table_tag = md_tag_from_string(node, str8("table"), 0);
                if (!md_node_is_nil(table_tag))
                {
                    MDG_Node_Grid *table = arena_push(arena, MDG_Node_Grid, 1);
                    MDG_Column_Desc_Array *col_descs = arena_push(arena, MDG_Column_Desc_Array, 1);
                    *table = mdg_node_grid_make_from_node(node, arena);
                    *col_descs = mdg_column_desc_array_from_tag(table_tag, arena);
                    mdg_map_insert_ptr(&table_grid_map, node->string, table, arena);
                    mdg_map_insert_ptr(&table_col_map, node->string, col_descs, arena);
                    table_count += 1;
                }
            }
        }
    }
    fmt_printfln(" %zu tables found", table_count);
    
    //- ak: gather layer options ==============================================
    for (MDG_ParsedFile_Node *n = parses.first; n != 0; n = n->next)
    {
        MD_Node *file = n->v.root;
        Str8 layer_key = mdg_layer_key_from_path(file->string, src_path, arena);
        MDG_Layer *layer = mdg_layer_from_key(state, layer_key, arena);
        for (MD_Node *node = file->first; !md_node_is_nil(node); node = node->next)
        {
            if (md_node_has_tag(node, str8("option"), 0))
            {
                if (str8_match(node->string, str8("library"), 0))
                {
                    layer->is_library = 1;
                }
            }
            if (md_node_has_tag(node, str8("gen_folder"), 0))
            {
                layer->gen_folder_name = node->string;
            }
            if (md_node_has_tag(node, str8("h_name"), 0))
            {
                layer->h_name_override = node->string;
            }
            if (md_node_has_tag(node, str8("c_name"), 0))
            {
                layer->c_name_override = node->string;
            }
            if (md_node_has_tag(node, str8("h_header"), 0))
            {
                Str8_List gen_strings = mdg_str_list_from_table_gen(table_grid_map, table_col_map, str8(""), node, arena);
                for (Str8_Node *n = gen_strings.first; n != 0; n = n->next)
                {
                    str8_list_push(arena, &layer->h_header, n->str);
                    str8_list_push(arena, &layer->h_header, str8("\n"));
                }
            }
            if (md_node_has_tag(node, str8("h_footer"), 0))
            {
                Str8_List gen_strings = mdg_str_list_from_table_gen(table_grid_map, table_col_map, str8(""), node, arena);
                for (Str8_Node *n = gen_strings.first; n != 0; n = n->next)
                {
                    str8_list_push(arena, &layer->h_footer, n->str);
                    str8_list_push(arena, &layer->h_footer, str8("\n"));
                }
            }
            if (md_node_has_tag(node, str8("c_header"), 0))
            {
                Str8_List gen_strings = mdg_str_list_from_table_gen(table_grid_map, table_col_map, str8(""), node, arena);
                for (Str8_Node *n = gen_strings.first; n != 0; n = n->next)
                {
                    str8_list_push(arena, &layer->c_header, n->str);
                    str8_list_push(arena, &layer->c_header, str8("\n"));
                }
            }
            if (md_node_has_tag(node, str8("c_footer"), 0))
            {
                Str8_List gen_strings = mdg_str_list_from_table_gen(table_grid_map, table_col_map, str8(""), node, arena);
                for (Str8_Node *n = gen_strings.first; n != 0; n = n->next)
                {
                    str8_list_push(arena, &layer->c_footer, n->str);
                    str8_list_push(arena, &layer->c_footer, str8("\n"));
                }
            }
        }
    }
    
    //- ak: generate enums ====================================================
    for (MDG_ParsedFile_Node *n = parses.first; n != 0; n = n->next)
    {
        MD_Node *file = n->v.root;
        for (MD_Node *node = file->first; !md_node_is_nil(node); node = node->next)
        {
            MD_Node *tag = md_tag_from_string(node, str8("enum"), 0);
            if (!md_node_is_nil(tag))
            {
                Str8 enum_name = node->string;
                Str8 enum_member_prefix = enum_name;
                if (str8_match(str8_postfix(enum_name, 5), str8("Flags"), 0))
                {
                    enum_member_prefix = str8_chop(enum_name, 1);
                }
                Str8 enum_base_type_name = tag->first->string;
                Str8 layer_key = mdg_layer_key_from_path(file->string, src_path, arena);
                MDG_Layer *layer = mdg_layer_from_key(state, layer_key, arena);
                Str8_List gen_strings = mdg_str_list_from_table_gen(table_grid_map, table_col_map, str8(""), node, arena);
                if (enum_base_type_name.size == 0)
                {
                    str8_list_pushf(arena, &layer->enums, "typedef enum %.*s\n{\n", str8_varg(enum_name));
                }
                else
                {
                    str8_list_pushf(arena, &layer->enums, "typedef %.*s %.*s;\n", str8_varg(enum_base_type_name), str8_varg(enum_name));
                    str8_list_pushf(arena, &layer->enums, "typedef enum %.*sEnum\n{\n", str8_varg(enum_name));
                }
                for (Str8_Node *n = gen_strings.first; n != 0; n = n->next)
                {
                    str8_list_pushf(arena, &layer->enums, "%.*s_%.*s,\n", str8_varg(enum_member_prefix), str8_varg(n->str));
                }
                if (enum_base_type_name.size == 0)
                {
                    str8_list_pushf(arena, &layer->enums, "} %.*s;\n\n", str8_varg(enum_name));
                }
                else
                {
                    str8_list_pushf(arena, &layer->enums, "} %.*sEnum;\n\n", str8_varg(enum_name));
                }
            }
        }
    }
    
    //- a: write all layer output files ======================================
    fmt_print("generating layer code...");
    for (size_t slot_idx = 0; slot_idx < state->slots_count; slot_idx += 1)
    {
        MDG_Layer_Slot *slot = &state->slots[slot_idx];
        for (MDG_Layer_Node *n = slot->first; n != 0; n = n->next)
        {
            MDG_Layer *layer = &n->v;
            Str8 layer_generated_folder = ZERO_STRUCT;
            if (layer->gen_folder_name.size != 0)
            {
                Str8 gen_folder = layer->gen_folder_name;
                layer_generated_folder = str8f(arena, "%.*s/%.*s", str8_varg(src_path), str8_varg(gen_folder));
            }
            else
            {
                Str8 gen_folder = defulat_gen_dirname;
                layer_generated_folder = str8f(arena, "%.*s/%.*s/%.*s", str8_varg(src_path), str8_varg(layer->key), str8_varg(gen_folder));
            }
            if (os_dir_ensure(layer_generated_folder))
            {
                Str8_List layer_key_parts = str8_split_path(arena, layer->key);
                Str_Join join = ZERO_STRUCT;
                join.sep = str8("_");
                Str8 layer_key_filename = str8_list_join(arena, &layer_key_parts, &join);
                Str8 layer_key_filename_upper = str8_to_upper(layer_key_filename, arena);
                Str8 h_path = str8f(arena, "%.*s/%.*s.meta.h", str8_varg(layer_generated_folder), str8_varg(layer_key_filename));
                Str8 c_path = str8f(arena, "%.*s/%.*s.meta.c", str8_varg(layer_generated_folder), str8_varg(layer_key_filename));
                if (layer->h_name_override.size != 0)
                {
                    h_path = str8f(arena, "%.*s/%.*s", str8_varg(layer_generated_folder), str8_varg(str8_skip_last_slash(layer->h_name_override)));
                }
                if (layer->c_name_override.size != 0)
                {
                    c_path = str8f(arena, "%.*s/%.*s", str8_varg(layer_generated_folder), str8_varg(str8_skip_last_slash(layer->c_name_override)));
                }
                {
                    Os_File h_file = os_file_open(h_path, Os_AccessFlag_Write);
                    if (layer->h_header.first == 0)
                    {
                        fmt_fprintfln(h_file, "//- GENERATED CODE\n");
                        fmt_fprintfln(h_file, "#ifndef %.*s_META_H", str8_varg(layer_key_filename_upper));
                        fmt_fprintfln(h_file, "#define %.*s_META_H\n", str8_varg(layer_key_filename_upper));
                    }
                    else for (Str8_Node *n = layer->h_header.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    for (Str8_Node *n = layer->enums.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    for (Str8_Node *n = layer->structs.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    for (Str8_Node *n = layer->h_catchall.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    for (Str8_Node *n = layer->h_functions.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    if (layer->h_tables.first != 0)
                    {
                        if (!layer->is_library)
                        {
                            fmt_fprintf(h_file, "C_LINKAGE_BEGIN\n");
                        }
                        for (Str8_Node *n = layer->h_tables.first; n != 0; n = n->next)
                        {
                            os_file_write_append(h_file, n->str.cstr, n->str.size);
                        }
                        fmt_fprintf(h_file, "\n");
                        if (!layer->is_library)
                        {
                            fmt_fprintf(h_file, "C_LINKAGE_END\n\n");
                        }
                    }
                    if (layer->h_footer.first == 0)
                    {
                        fmt_fprintf(h_file, "#endif // %.*s_META_H\n", str8_varg(layer_key_filename_upper));
                    }
                    else for (Str8_Node *n = layer->h_footer.first; n != 0; n = n->next)
                    {
                        os_file_write_append(h_file, n->str.cstr, n->str.size);
                    }
                    os_file_close(h_file);
                }
                {
                    Os_File c_file = os_file_open(c_path, Os_AccessFlag_Write);
                    if (layer->c_header.first == 0)
                    {
                        fmt_fprintfln(c_file, "//- GENERATED CODE\n");
                    }
                    else for (Str8_Node *n = layer->c_header.first; n != 0; n = n->next)
                    {
                        os_file_write_append(c_file, n->str.cstr, n->str.size);
                    }
                    for (Str8_Node *n = layer->c_catchall.first; n != 0; n = n->next)
                    {
                        os_file_write_append(c_file, n->str.cstr, n->str.size);
                    }
                    if (layer->c_tables.first != 0)
                    {
                        if (!layer->is_library)
                        {
                            fmt_fprintfln(c_file, "C_LINKAGE_BEGIN");
                        }
                        for (Str8_Node *n = layer->c_tables.first; n != 0; n = n->next)
                        {
                            os_file_write_append(c_file, n->str.cstr, n->str.size);
                        }
                        if (!layer->is_library)
                        {
                            fmt_fprintfln(c_file, "C_LINKAGE_END\n");
                        }
                    }
                    for (Str8_Node *n = layer->c_functions.first; n != 0; n = n->next)
                    {
                        os_file_write_append(c_file, n->str.cstr, n->str.size);
                    }
                    if (layer->c_footer.first != 0)
                    {
                        for (Str8_Node *n = layer->c_footer.first; n != 0; n = n->next)
                        {
                            os_file_write_append(c_file, n->str.cstr, n->str.size);
                        }
                    }
                    os_file_close(c_file);
                }
            }
        }
    }
    fmt_print("\n");
    
    //- ak: write out all messages to stderr ==================================
    Log_Context context   = log_init();
    char *file_info_color = log_get_file_info_color(&context);
    char *restart_color   = log_get_reset_color(&context);
    for (MDG_Msg_Node *n = msgs.first; n != 0; n = n->next)
    {
        MDG_Msg *msg = &n->v;
        fmt_eprintf("%s%.*s:%ld:%ld%s ", file_info_color, str8_varg(msg->file_path), msg->pt.line, msg->pt.column, restart_color);
        log_print_color_level(&context, msg->level);
        fmt_eprintf("%.*s\n", str8_varg(msg->string));
    }
}
