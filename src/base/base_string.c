// Character Classification & Conversion Functions
//=============================================================================

internal bool
char_is_space(U8 c)
{
    return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

internal bool
char_is_upper(U8 c)
{
    return('A' <= c && c <= 'Z');
}

internal bool
char_is_lower(U8 c)
{
    return('a' <= c && c <= 'z');
}

internal bool
char_is_alpha(U8 c)
{
    return(char_is_upper(c) || char_is_lower(c));
}

internal bool
char_is_slash(U8 c)
{
    return(c == '/' || c == '\\');
}

internal U8
char_to_lower(U8 c)
{
    if (char_is_upper(c))
    {
        c += ('a' - 'A');
    }
    return(c);
}

internal U8
char_to_upper(U8 c)
{
    if (char_is_lower(c))
    {
        c += ('A' - 'a');
    }
    return(c);
}

internal U8
char_to_correct_slash(U8 c)
{
    if(char_is_slash(c))
    {
        c = '/';
    }
    return(c);
}

// C-String Measurement
//=============================================================================

internal U64
cstr8_length(U8 *c)
{
    U8 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}
internal U64
cstr16_length(U16 *c)
{
    U16 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}

internal U64
cstr32_length(U32 *c)
{
    U32 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}

// String Constructors
//=============================================================================

internal Str8
str8_init(U8 *str, U64 size) {
    return (Str8){str, size};
}
internal Str8
str8_from_cstr(char *c) {
    return (Str8){(U8*)c, cstr8_length((U8*)c)};
}

internal Str16
str16_init(U16 *str, U64 size) {
    return (Str16){str, size};
}
internal Str16
str16_from_cstr(U16 *c) {
    return (Str16){(U16*)c, cstr16_length((U16*)c)};
}

internal Str32
str32_init(U32 *str, U64 size) {
    return (Str32){str, size};
}
internal Str32
str32_from_cstr(U32 *c) {
    return (Str32){(U32*)c, cstr32_length((U32*)c)};
}

internal Str8
str8_range(U8 *first, U8 *one_past_last)
{
    Str8 result = {first, (U64)(one_past_last - first)};
    return result;
}


// String Matching
//=============================================================================

internal bool
str8_match(Str8 a, Str8 b, StrMatchFlags flags)
{
    bool result = 0;
    if (a.size == b.size)
    {
        if(flags == 0) {
            result = mem_match(a.str, b.str, b.size);
        } else if(flags & StrMatchFlag_RightSideSloppy) {
            bool case_insensitive  = (flags & StrMatchFlag_CaseInsensitive);
            bool slash_insensitive = (flags & StrMatchFlag_SlashInsensitive);
            U64 size              = Min(a.size, b.size);
            result = 1;
            for(U64 i = 0; i < size; i += 1)
            {
                U8 at = a.str[i];
                U8 bt = b.str[i];
                if(case_insensitive)
                {
                    at = char_to_upper(at);
                    bt = char_to_upper(bt);
                }
                if(slash_insensitive)
                {
                    at = char_to_correct_slash(at);
                    bt = char_to_correct_slash(bt);
                }
                if(at != bt)
                {
                    result = 0;
                    break;
                }
            }
        }
    }
    return result;
}

internal bool
str8_ends_with(Str8 string, Str8 end)
{
    Str8 postfix = str8_postfix(string, end.size);
    bool is_match = str8_match(end, postfix, 0);
    return is_match;
}

internal U64
str8_find_substr(Str8 string, U64 start_pos, Str8 substr, StrMatchFlags flags)
{
    U8 *p = string.str + start_pos;
    U64 stop_offset = Max(string.size + 1, substr.size) - substr.size;
    U8 *stop_p = string.str + stop_offset;
    if (substr.size > 0)
    {
        U8 *string_opl = string.str + string.size;
        Str8 needle_tail = str8_skip(substr, 1);
        StrMatchFlags adjusted_flags = flags | StrMatchFlag_RightSideSloppy;
        U8 needle_first_char_adjusted = substr.str[0];
        if(adjusted_flags & StrMatchFlag_CaseInsensitive)
        {
            needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
        }
        for (;p < stop_p; p += 1)
        {
            U8 haystack_char_adjusted = *p;
            if(adjusted_flags & StrMatchFlag_CaseInsensitive)
            {
                haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
            }
            if (haystack_char_adjusted == needle_first_char_adjusted)
            {
                if (
                    str8_match(str8_range(p+1, string_opl), needle_tail, adjusted_flags)
                ){
                    break;
                }
            }
        }
    }
    U64 result = string.size;
    if (p < stop_p)
    {
        result = (U64)(p - string.str);
    }
    return(result);
}

internal U64
str8_find_substr_reverse(Str8 string, U64 start_pos, Str8 substr, StrMatchFlags flags)
{
  U64 result = 0;
  for(I64 i = string.size - start_pos - substr.size; i >= 0; --i)
  {
    Str8 haystack = str8_substr(string, rng_1u64(i, i + substr.size));
    if(str8_match(haystack, substr, flags))
    {
      result = (U64)i + substr.size;
      break;
    }
  }
  return result;
}

// String Slicing
//=============================================================================

internal Str8
str8_substr(Str8 str, Rng1U64 range)
{
    range.min = Min(range.min, str.size);
    range.max = Min(range.max, str.size);
    str.str += range.min;
    str.size = dim_1u64(range);
    return(str);
}

internal Str8
str8_postfix(Str8 str, U64 size)
{
    size = Min(size, str.size);
    str.str = (str.str + str.size) - size;
    str.size = size;
    return(str);
}

internal Str8
str8_prefix(Str8 str, U64 size)
{
    str.size = Min(size, str.size);
    return str;
}

internal Str8
str8_skip(Str8 str, U64 amt)
{
    amt = Min(amt, str.size);
    str.str += amt;
    str.size -= amt;
    return str;
}

internal Str8
str8_cat(Alloc alloc, Str8 s1, Str8 s2)
{
    Str8 str;
    str.size = s1.size + s2.size;
    str.str = alloc_make(alloc, U8, str.size + 1);
    mem_copy(str.str, s1.str, s1.size);
    mem_copy(str.str + s1.size, s2.str, s2.size);
    str.str[str.size] = 0;
    return(str);
}

// String List Construction Functions
//=============================================================================

internal Str8Node*
str8_list_push(Alloc alloc, Str8List *list, Str8 string)
{
    Str8Node *node = alloc_make(alloc, Str8Node, 1);
    SLLPush(list->first, list->last, node);
    list->count += 1;
    list->total_size += string.size;
    node->string = string;
    return(node);
}

// String Split and Join
//=============================================================================

internal Str8List
str8_split(
    Alloc alloc, Str8 string, U8 *split_chars, U64 split_char_count,
    StrSplitFlags flags
) {
    Str8List list = ZERO_STRUCT;
    bool keep_empties = (flags & StrSplitFlag_KeepEmpties);
    U8 *ptr = string.str;
    U8 *opl = string.str + string.size;
    for (;ptr < opl;)
    {
        U8 *first = ptr;
        for (;ptr < opl; ptr += 1)
        {
            U8 c = *ptr;
            bool is_split = 0;
            for (U64 i = 0; i < split_char_count; i += 1)
            {
                if (split_chars[i] == c)
                {
                    is_split = 1;
                    break;
                }
            }
            if (is_split)
            {
                break;
            }
        }
        Str8 string = str8_range(first, ptr);
        if (keep_empties || string.size > 0)
        {
            str8_list_push(alloc, &list, string);
        }
        ptr += 1;
    }
  return list;
}

internal Str8
str8_list_join(Alloc alloc, Str8List *list, StrJoin *optional_params)
{
    StrJoin join = {0};
    if (optional_params != 0)
    {
        MemoryCopyStruct(&join, optional_params);
    }
    U64 sep_count = 0;
    if (list->count > 0)
    {
        sep_count = list->count - 1;
    }
    Str8 result;
    result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list->total_size;
    U8 *ptr = result.str = alloc_make(alloc, U8, result.size + 1);
    mem_copy(ptr, join.pre.str, join.pre.size);
    ptr += join.pre.size;
    for (Str8Node *node = list->first; node != 0; node = node->next)
    {
        mem_copy(ptr, node->string.str, node->string.size);
        ptr += node->string.size;
        if (node->next != 0)
        {
            mem_copy(ptr, join.sep.str, join.sep.size);
            ptr += join.sep.size;
        }
    }
    mem_copy(ptr, join.post.str, join.post.size);
    ptr += join.post.size;
    *ptr = 0;
    return result;
}

// String Formatting & Copying
//=============================================================================

internal Str8
str8_copy(Alloc alloc, Str8 s)
{
    Str8 str;
    str.size = s.size;
    str.str = alloc_make(alloc, U8, str.size + 1);
    mem_copy(str.str, s.str, s.size);
    str.str[str.size] = 0;
    return(str);
}

// String Hash
//=============================================================================

internal U64
str8_hash_u64_from_seed(U64 seed, Str8 string)
{
    U64 result = XXH3_64bits_withSeed(string.str, string.size, seed);
    return result;
}

internal U64
str8_hash_u64(Str8 string)
{
    U64 result = str8_hash_u64_from_seed(5381, string);
    return result;
}
