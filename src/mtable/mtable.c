internal void mt_token_batch_list_push(MT_Token_Batch_List *list, size_t cap, MT_Token token, Arena *arena)
{
    MT_Token_Batch_Node *node = list->last;
    if(node == 0 || node->count >= node->cap)
    {
        node = arena_push(arena, MT_Token_Batch_Node, 1);
        node->cap = cap;
        node->v = arena_push_nz(arena, MT_Token, cap);
        SLLQueuePush(list->first, list->last, node);
        list->chunk_count += 1;
    }
    MemoryCopyStruct(&node->v[node->count], &token);
    node->count += 1;
    list->total_token_count += 1;
}

internal MT_Tokenize mt_tokenize_from_str8(Str8 text, Arena *arena)
{
    Arena_Temp scratch = arena_scratch_begin(&arena, 1);
    MT_Token_Batch_List tokens = {0};
    uint8_t *byte_first = text.cstr;
    uint8_t *byte_opl = byte_first + text.length;
    uint8_t *byte = byte_first;
    //- ak: scan string & produce tokens
    while (byte < byte_opl)
    {
        MT_Token_Flags token_flags = 0;
        uint8_t *token_start = 0;
        (void)token_start;
        uint8_t *token_opl = 0;
        //- ak: whitespace
        if(*byte == ' ' || *byte == '\t' || *byte == '\v' || *byte == '\r')
        {
            token_flags = MT_Token_Flag_Whitespace;
            token_start = byte;
            token_opl = byte;
            byte += 1;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl || (*byte != ' ' && *byte != '\t' && *byte != '\v' && *byte != '\r'))
                {
                    break;
                }
            }
        }
        //- ak: newlines
        else if(*byte == '\n')
        {
            token_flags = MT_Token_Flag_Newline;
            token_start = byte;
            token_opl = byte+1;
            byte += 1;
        }
        //- ak: single-line comments
        else if(byte+1 < byte_opl && *byte == '/' && byte[1] == '/')
        {
            token_flags = MT_Token_Flag_Comment;
            token_start = byte;
            token_opl = byte+2;
            byte += 2;
            bool escaped = 0;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl)
                {
                    break;
                }
                if(escaped)
                {
                    escaped = 0;
                }
                else
                {
                    if(*byte == '\n')
                    {
                        break;
                    }
                    else if(*byte == '\\')
                    {
                        escaped = 1;
                    }
                }
            }
        }
        //- ak: multi-line comments
        else if(byte+1 < byte_opl && *byte == '/' && byte[1] == '*')
        {
            token_flags = MT_Token_Flag_Comment;
            token_start = byte;
            token_opl = byte+2;
            byte += 2;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl)
                {
                    token_flags |= MT_Token_Flag_BrokenComment;
                    break;
                }
                if(byte+1 < byte_opl && byte[0] == '*' && byte[1] == '/')
                {
                    token_opl += 2;
                    break;
                }
            }
        }
        //- ak: identifiers
        else if(('A' <= *byte && *byte <= 'Z') ||
                ('a' <= *byte && *byte <= 'z') ||
                *byte == '_' || utf8_class[*byte>>3] >= 2 )
        {
            token_flags = MT_Token_Flag_Identifier;
            token_start = byte;
            token_opl = byte;
            byte += 1;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl ||
                    (!('A' <= *byte && *byte <= 'Z') &&
                    !('a' <= *byte && *byte <= 'z')  &&
                    !('0' <= *byte && *byte <= '9')  &&
                    *byte != '_' && utf8_class[*byte>>3] < 2))
                {
                    break;
                }
            }
        }
        //- ak: numerics
        else if(('0' <= *byte && *byte <= '9') ||
            (*byte == '.' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9') ||
            (*byte == '-' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9') ||
            *byte == '_')
        {
            token_flags = MT_Token_Flag_Numeric;
            token_start = byte;
            token_opl = byte;
            byte += 1;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl ||
                    (!('A' <= *byte && *byte <= 'Z') &&
                     !('a' <= *byte && *byte <= 'z') &&
                     !('0' <= *byte && *byte <= '9') &&
                     *byte != '_' &&
                     *byte != '.'))
                {
                    break;
                }
            }
        }
        //- ak: singlet string literals
        else if(byte[0] == '"' || byte[0] == '\'')
        {
            uint8_t literal_style = byte[0];
            token_flags = MT_Token_Flag_StringLiteral;
            token_flags |= (literal_style == '\'')*MT_Token_Flag_StringSingleQuote;
            token_flags |= (literal_style ==  '"')*MT_Token_Flag_StringDoubleQuote;
            token_start = byte;
            token_opl = byte+1;
            byte += 1;
            bool escaped = 0;
            for(;byte <= byte_opl; byte += 1)
            {
                if(byte == byte_opl || *byte == '\n')
                {
                    token_opl = byte;
                    token_flags |= MT_Token_Flag_BrokenStringLiteral;
                    break;
                }
                if(!escaped && byte[0] == '\\')
                {
                    escaped = 1;
                }
                else if(!escaped && byte[0] == literal_style)
                {
                    token_opl = byte+1;
                    byte += 1;
                    break;
                }
                else if(escaped)
                {
                    escaped = 0;
                }
            }
        }
        //- ak: non-reserved symbols
        else if(*byte == '~' || *byte == '!' || *byte == '$' || *byte == '%' || *byte == '^'  ||
            *byte == '&' || *byte == '*' || *byte == '-' || *byte == '=' || *byte == '+' ||
            *byte == '<' || *byte == '.' || *byte == '>' || *byte == '/' || *byte == '?' ||
            *byte == '|')
        {
            token_flags = MT_Token_Flag_Symbol;
            token_start = byte;
            token_opl = byte;
            byte += 1;
            for(;byte <= byte_opl; byte += 1)
            {
                token_opl += 1;
                if(byte == byte_opl ||
                        (*byte != '~' && *byte != '!' && *byte != '$' && *byte != '%' && *byte != '^' &&
                         *byte != '&' && *byte != '*' && *byte != '-' && *byte != '=' && *byte != '+' &&
                         *byte != '<' && *byte != '.' && *byte != '>' && *byte != '/' && *byte != '?' &&
                         *byte != '|'))
                {
                    break;
                }
            }
        }
        //- ak: reserved symbols
        else if(*byte == '{' || *byte == '}' || *byte == '(' || *byte == ')'  ||
            *byte == '[' || *byte == ']' || *byte == '#' || *byte == ',' ||
            *byte == '\\'|| *byte == ':' || *byte == ';' || *byte == '@')
        {
            token_flags = MT_Token_Flag_Reserved;
            token_start = byte;
            token_opl = byte+1;
            byte += 1;
        }
        //- ak: bad characters in all other cases
        else
        {
            token_flags = MT_Token_Flag_BadCharacter;
            token_start = byte;
            token_opl = byte+1;
            byte += 1;
        }
        
        //- ak; push token if formed
        if(token_flags != 0 && token_start != 0 && token_opl > token_start)
        {
            MT_Token token = (MT_Token){
                {
                    (uint64_t)(token_start - byte_first),
                    (uint64_t)(token_opl - byte_first)
                },
                token_flags
            };
            mt_token_batch_list_push(&tokens, 4096, token, scratch.arena);
        }
        // TODO(aman.v): handle error
        //- ak: push errors on unterminated comments
        if(token_flags & MT_Token_Flag_BrokenComment)
        {
            // MD_Node *error = md_push_node(arena, MD_NodeKind_ErrorMarker, 0, str8_lit(""), str8_lit(""), token_start - byte_first);
            // String8 error_string = str8_lit("Unterminated comment.");
            // md_msg_list_push(arena, &msgs, error, MD_MsgKind_Error, error_string);
        }
        //- ak: push errors on unterminated strings
        if(token_flags & MT_Token_Flag_BrokenStringLiteral)
        {
            // MD_Node *error = md_push_node(arena, MD_NodeKind_ErrorMarker, 0, str8_lit(""), str8_lit(""), token_start - byte_first);
            // String8 error_string = str8_lit("Unterminated string literal.");
            // md_msg_list_push(arena, &msgs, error, MD_MsgKind_Error, error_string);
        }
    }
    arena_scratch_end(scratch);
    MT_Tokenize result = ZERO_STRUCT;
    return result;
}

// MT_Parse mt_parse_from_tokens(MT_Token_Array tokens, Str8 text, Str8 path, Arena *arena)
// {
//     MD_Parse parse = ZERO_STRUCT;
//     return parse;
// }
