// Character Classification & Conversion Functions
//=============================================================================

internal bool char_is_space(uint8_t c)
{
    return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}
internal uint8_t char_to_correct_slash(uint8_t c)
{
    if(char_is_slash(c))
    {
        c = '/';
    }
    return(c);
}

internal bool char_is_upper(uint8_t c)
{
    return('A' <= c && c <= 'Z');
}
internal bool char_is_lower(uint8_t c)
{
    return('a' <= c && c <= 'z');
}

internal bool char_is_alpha(uint8_t c)
{
    return(char_is_upper(c) || char_is_lower(c));
}

internal bool char_is_slash(uint8_t c)
{
    return(c == '/' || c == '\\');
}

internal uint8_t char_to_lower(uint8_t c)
{
    if (char_is_upper(c))
    {
        c += ('a' - 'A');
    }
    return(c);
}
internal uint8_t char_to_upper(uint8_t c)
{
    if (char_is_lower(c))
    {
        c += ('A' - 'a');
    }
    return(c);
}

internal bool char_is_digit(uint8_t c, uint32_t base)
{
    bool result = 0;
    if(0 < base && base <= 16)
    {
        uint8_t val = integer_symbol_reverse[c];
        if(val < base)
        {
            result = 1;
        }
    }
    return result;
}

// C-String Measurement
//=============================================================================

internal uint64_t cstr8_length(uint8_t *c)
{
    uint8_t *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}
internal uint64_t cstr16_length(uint16_t *c)
{
    uint16_t *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}
internal uint64_t cstr32_length(uint32_t *c)
{
    uint32_t *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}

// String Constructors
//=============================================================================

internal Str8 str8_init(uint8_t *str, uint64_t size)
{
    return (Str8){str, size};
}
internal Str8 str8_from_cstr(char *c)
{
    return (Str8){(uint8_t*)c, cstr8_length((uint8_t*)c)};
}

internal Str16 str16_init(uint16_t *str, uint64_t size)
{
    return (Str16){str, size};
}
internal Str16 str16_from_cstr(uint16_t *c)
{
    return (Str16){(uint16_t*)c, cstr16_length((uint16_t*)c)};
}

internal Str32 str32_init(uint32_t *str, uint64_t size)
{
    return (Str32){str, size};
}
internal Str32 str32_from_cstr(uint32_t *c)
{
    return (Str32){(uint32_t*)c, cstr32_length((uint32_t*)c)};
}

internal Str8 str8_range(uint8_t *first, uint8_t *one_past_last)
{
    return (Str8){first, (uint64_t)(one_past_last - first)};
}

// String Matching
//=============================================================================

internal bool str8_match(Str8 a, Str8 b, StrMatchFlags flags)
{
    bool result = 0;
    if (a.size == b.size && flags == 0)
    {
        result = mem_match(a.cstr, b.cstr, b.size);
    }
    else if (a.size == b.size || (flags & StrMatchFlag_RightSideSloppy))
    {
        bool case_insensitive  = (flags & StrMatchFlag_CaseInsensitive);
        bool slash_insensitive = (flags & StrMatchFlag_SlashInsensitive);
        uint64_t size               = Min(a.size, b.size);
        result = 1;
        for(uint64_t i = 0; i < size; i += 1)
        {
            uint8_t at = a.cstr[i];
            uint8_t bt = b.cstr[i];
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
    return result;
}

internal bool str8_ends_with(Str8 string, Str8 end)
{
    Str8 postfix = str8_postfix(string, end.size);
    bool is_match = str8_match(end, postfix, 0);
    return is_match;
}

internal uint64_t str8_find_substr(Str8 string, uint64_t start_pos, Str8 substr, StrMatchFlags flags) {
    uint8_t *p = string.cstr + start_pos;
    uint64_t stop_offset = Max(string.size + 1, substr.size) - substr.size;
    uint8_t *stop_p = string.cstr + stop_offset;
    if (substr.size > 0)
    {
        uint8_t *string_opl = string.cstr + string.size;
        Str8 needle_tail = str8_skip(substr, 1);
        StrMatchFlags adjusted_flags = flags | StrMatchFlag_RightSideSloppy;
        uint8_t needle_first_char_adjusted = substr.cstr[0];
        if(adjusted_flags & StrMatchFlag_CaseInsensitive)
        {
            needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
        }
        for (;p < stop_p; p += 1)
        {
            uint8_t haystack_char_adjusted = *p;
            if(adjusted_flags & StrMatchFlag_CaseInsensitive)
            {
                haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
            }
            if (haystack_char_adjusted == needle_first_char_adjusted)
            {
                if (str8_match(str8_range(p+1, string_opl), needle_tail, adjusted_flags))
                {
                    break;
                }
            }
        }
    }
    uint64_t result = string.size;
    if (p < stop_p)
    {
        result = (uint64_t)(p - string.cstr);
    }
    return result;
}

internal uint64_t str8_find_substr_reverse(Str8 string, uint64_t start_pos, Str8 substr, StrMatchFlags flags) {
    uint64_t result = 0;
    for(int64_t i = string.size - start_pos - substr.size; i >= 0; --i)
    {
        Str8 haystack = str8_substr(string, rng1_u64(i, i + substr.size));
        if(str8_match(haystack, substr, flags))
        {
            result = (uint64_t)i + substr.size;
            break;
        }
    }
    return result;
}

// String Slicing
//=============================================================================

internal Str8 str8_substr(Str8 str, Rng1_U64 range)
{
    range.min = Min(range.min, str.size);
    range.max = Min(range.max, str.size);
    str.cstr += range.min;
    str.size = dim1_u64(range);
    return(str);
}

internal Str8 str8_postfix(Str8 str, uint64_t size)
{
    size = Min(size, str.size);
    str.cstr = (str.cstr + str.size) - size;
    str.size = size;
    return(str);
}

internal Str8 str8_prefix(Str8 str, uint64_t size)
{
    str.size = Min(size, str.size);
    return str;
}

internal Str8 str8_skip(Str8 str, uint64_t amt)
{
    amt = Min(amt, str.size);
    str.cstr += amt;
    str.size -= amt;
    return str;
}

internal Str8 str8_cat(Alloc alloc, Str8 s1, Str8 s2)
{
    Str8 str;
    str.size = s1.size + s2.size;
    str.cstr = alloc_make(alloc, uint8_t, str.size + 1);
    mem_copy(str.cstr, s1.cstr, s1.size);
    mem_copy(str.cstr + s1.size, s2.cstr, s2.size);
    str.cstr[str.size] = 0;
    return(str);
}

// String Conversions
//=============================================================================

internal int64_t str8_to_sign(Str8 string, Str8 *string_tail)
{
    // count negative signs
    uint64_t neg_count = 0;
    uint64_t i = 0;
    for(; i < string.size; i += 1)
    {
        if (string.cstr[i] == '-'){
            neg_count += 1;
        }
        else if (string.cstr[i] != '+'){
            break;
        }
    }

    // output part of string after signs
    *string_tail = str8_skip(string, i);

    // output integer sign
    int64_t sign = (neg_count & 1)?-1:+1;
    return sign;
}

internal bool str8_is_integer(Str8 string, uint32_t radix)
{
  bool result = false;
  Str8 sign = str8_prefix(string, 1);
  if(str8_match(sign, str8("-"), 0))
  {
    result = str8_is_integer_unsigned(str8_skip(string, 1), radix);
  }
  else
  {
    result = str8_is_integer_unsigned(string, radix);
  }
  return result;
}


internal bool str8_is_integer_unsigned(Str8 string, uint32_t radix)
{
    bool result = false;
    if(string.size > 0)
    {
        if(1 < radix && radix <= 16)
        {
            result = true;
            for(uint64_t i = 0; i < string.size; i += 1)
            {
                uint8_t c = string.cstr[i];
                if(!(c < 0x80) || integer_symbol_reverse[c] >= radix)
                {
                    result = false;
                    break;
                }
            }
        }
    }
    return result;
}

internal bool str8_is_float(Str8 string)
{
    if (string.size == 0) {
        return false;
    }
    uint64_t i = 0;
    if (i < string.size && (string.cstr[i] == '+' || string.cstr[i] == '-'))
    {
        i += 1;
    }
    bool has_digits = false;
    bool has_dot = false;
    bool has_exp = false;
    while (i < string.size)
    {
        uint8_t c = string.cstr[i];
        if (c >= '0' && c <= '9')
        {
            has_digits = true;
        }
        else if (c == '.' && !has_dot && !has_exp)
        {
            has_dot = true;
        }
        else if ((c == 'e' || c == 'E') && !has_exp && has_digits)
        {
            // Exponent allowed only after at least one digit
            has_exp = true;
            i += 1;
            if (i >= string.size)
            {
                return false; // 'e' at end is invalid
            }
            // Optional exponent sign
            if (string.cstr[i] == '+' || string.cstr[i] == '-')
            {
                i += 1;
            }
            // Must have at least one digit in exponent
            if (i >= string.size || string.cstr[i] < '0' || string.cstr[i] > '9')
            {
                return false;
            }
            // Skip remaining exponent digits
            while (i < string.size && string.cstr[i] >= '0' && string.cstr[i] <= '9')
            {
                i += 1;
            }
            return i == string.size; // Must consume all after 'e'
        }
        else
        {
            return false;
        }
        i += 1;
    }
    // Valid if we saw at least one digit (somewhere before or after dot)
    // Examples: ".5" → valid, "5." → valid, "." → invalid
    return has_digits;
}

internal uint64_t str8_to_u64(Str8 string, uint32_t radix)
{
    uint64_t x = 0;
    if(1 < radix && radix <= 16)
    {
        for(uint64_t i = 0; i < string.size; i += 1)
        {
            x *= radix;
            x += integer_symbol_reverse[string.cstr[i]&0x7F];
        }
    }
    return x;
}

internal uint32_t str8_to_u32(Str8 string, uint32_t radix)
{
    uint64_t x64 = str8_to_u64(string, radix);
    uint32_t x32 = safe_cast_u32(x64);
    return x32;
}

internal int64_t str8_to_i64(Str8 string, uint32_t radix)
{
    int64_t sign = str8_to_sign(string, &string);
    int64_t x = (int64_t)str8_to_u64(string, radix) * sign;
    return x;
}

internal int32_t str8_to_i32(Str8 string, uint32_t radix)
{
    int64_t x64 = str8_to_i64(string, radix);
    int32_t x32 = safe_cast_s32(x64);
    return x32;
}

internal double str8_to_f64(Str8 string)
{
    // TODO(rjf): crappy implementation for now that just uses atof.
    double result = 0;
    if(string.size > 0)
    {
        // Find starting pos of numeric string, as well as sign
        double sign = +1.0;
        if(string.cstr[0] == '-')
        {
            sign = -1.0;
        }
        else if(string.cstr[0] == '+')
        {
            sign = 1.0;
        }
        // Gather numerics
        uint64_t num_valid_chars = 0;
        char buffer[64];
        bool exp = 0;
        for(uint64_t idx = 0; idx < string.size && num_valid_chars < sizeof(buffer)-1; idx += 1)
        {
            if(char_is_digit(string.cstr[idx], 10) || string.cstr[idx] == '.' || string.cstr[idx] == 'e' ||
                    (exp && (string.cstr[idx] == '+' || string.cstr[idx] == '-')))
            {
                buffer[num_valid_chars] = string.cstr[idx];
                num_valid_chars += 1;
                exp = 0;
                exp = (string.cstr[idx] == 'e');
            }
        }
        // Null-terminate.
        buffer[num_valid_chars] = 0;
        // Do final conversion
        result = sign * atof(buffer);
    }
    return result;
}

internal Str8 str8_from_bool(bool value)
{
    Str8 result = value ? str8("true") : str8("false");
    return result;
}
internal bool str8_is_bool(Str8 str)
{
    bool result = str8_match(str, str8("true"), 0) || str8_match(str, str8("false"), 0);
    return result;
}
internal bool str8_to_bool(Str8 str)
{
    bool result = str8_match(str, str8("true"), 0) ? true : false;
    return result;
}

// String List Construction Functions
//=============================================================================

internal Str8Node* str8_list_push(Alloc alloc, Str8List *list, Str8 string)
{
    Str8Node *node = alloc_make(alloc, Str8Node, 1);
    SLLQueuePush(list->first, list->last, node);
    list->count += 1;
    list->size += string.size;
    node->string = string;
    return node;
}

// String Arrays
// ==============================================================

internal Str8Array str8_array_alloc(Alloc alloc, uint64_t size)
{
    Str8Array arr;
    arr.size = size;
    arr.count = 0;
    arr.strings = alloc_make(alloc, Str8, size);
    return arr;
}
internal Str8 *str8_array_append(Str8Array *array, Str8 str)
{
    Str8 *result = NULL;
    array->strings[array->count] = str;
    result = &array->strings[array->count];
    array->count++;
    return result;
}
internal Str8 *str8_array_get(Str8Array *array, uint64_t index)
{
    Str8 *result = NULL;
    if (array->count >= index)
    {
        result = &array->strings[index];
    }
    return result;
}
internal Str8Array str8_array_from_list(Alloc alloc, Str8List *list)
{
    Str8Array array;
    array.size = list->count;
    array.count = array.size;
    array.strings = alloc_make(alloc, Str8, array.size);
    uint64_t idx = 0;
    for (Str8Node *n = list->first; n != 0; n = n->next, idx += 1)
    {
        array.strings[idx] = n->string;
    }
    return array;
}

// String Split and Join
//=============================================================================

internal Str8List str8_split(Alloc alloc, Str8 string, uint8_t *split_chars, uint64_t split_char_count, StrSplitFlags flags)
{
    Str8List list = ZERO_STRUCT;
    bool keep_empties = (flags & StrSplitFlag_KeepEmpties);
    uint8_t *ptr = string.cstr;
    uint8_t *opl = string.cstr + string.size;
    for (;ptr < opl;)
    {
        uint8_t *first = ptr;
        for (;ptr < opl; ptr += 1)
        {
            uint8_t c = *ptr;
            bool is_split = false;
            for (uint64_t i = 0; i < split_char_count; i += 1)
            {
                if (split_chars[i] == c)
                {
                    is_split = true;
                    break;
                }
            }
            if (is_split)
            {
                break;
            }
        }
        Str8 node = str8_range(first, ptr);
        if (keep_empties || node.size > 0)
        {
            str8_list_push(alloc, &list, node);
        }
        ptr += 1;
    }
  return list;
}

internal Str8 str8_list_join(Alloc alloc, Str8List *list, StrJoin *optional_params)
{
    StrJoin join = {0};
    if (optional_params != 0)
    {
        MemoryCopyStruct(&join, optional_params);
    }
    uint64_t sep_count = 0;
    if (list->count > 0)
    {
        sep_count = list->count - 1;
    }
    Str8 result;
    result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list->size;
    uint8_t *ptr = result.cstr = alloc_make(alloc, uint8_t, result.size + 1);
    mem_copy(ptr, join.pre.cstr, join.pre.size);
    ptr += join.pre.size;
    for (Str8Node *node = list->first; node != 0; node = node->next)
    {
        mem_copy(ptr, node->string.cstr, node->string.size);
        ptr += node->string.size;
        if (node->next != 0)
        {
            mem_copy(ptr, join.sep.cstr, join.sep.size);
            ptr += join.sep.size;
        }
    }
    mem_copy(ptr, join.post.cstr, join.post.size);
    ptr += join.post.size;
    *ptr = 0;
    return result;
}

// String Formatting & Copying
//=============================================================================

internal Str8 str8_copy(Alloc alloc, Str8 s)
{
    Str8 str;
    str.size = s.size;
    str.cstr = alloc_make(alloc, uint8_t, str.size + 1);
    mem_copy(str.cstr, s.cstr, s.size);
    str.cstr[str.size] = 0;
    return(str);
}

internal Str8 str8fv(Alloc alloc, char *fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);
    uint32_t needed_bytes = fmt_vsnprintf(0, 0, fmt, args) + 1;
    Str8 result = ZERO_STRUCT;
    result.cstr = alloc_make(alloc, uint8_t, needed_bytes);
    result.size = fmt_vsnprintf((char*)result.cstr, needed_bytes, fmt, args2);
    result.cstr[result.size] = 0;
    va_end(args2);
    return result;
}

internal Str8 str8f(Alloc alloc, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Str8 result = str8fv(alloc, fmt, args);
    va_end(args);
    return result;
}

// UTF-8 & UTF-16 Decoding/Encoding
//=============================================================================

internal UnicodeDecode utf8_decode(uint8_t *str, uint64_t max)
{
    UnicodeDecode result = {1, max_u32};
    uint8_t byte = str[0];
    uint8_t byte_class = utf8_class[byte >> 3];
    switch (byte_class)
    {
        case 1:{
            result.codepoint = byte;
        }break;
        case 2: {
            if (1 < max)
            {
                uint8_t cont_byte = str[1];
                if (utf8_class[cont_byte >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask5) << 6;
                    result.codepoint |=  (cont_byte & bitmask6);
                    result.inc = 2;
                }
            }
        }break;
        case 3:
        {
            if (2 < max)
            {
                uint8_t cont_byte[2] = {str[1], str[2]};
                if (
                    utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0
                ) {
                    result.codepoint = (byte & bitmask4) << 12;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
                    result.codepoint |=  (cont_byte[1] & bitmask6);
                    result.inc = 3;
                }
            }
        }break;
        case 4:
        {
            if (3 < max)
            {
                uint8_t cont_byte[3] = {str[1], str[2], str[3]};
                if (
                    utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0 &&
                    utf8_class[cont_byte[2] >> 3] == 0
                ) {
                    result.codepoint = (byte & bitmask3) << 18;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
                    result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
                    result.codepoint |=  (cont_byte[2] & bitmask6);
                    result.inc = 4;
                }
            }
        }
    }
    return result;
}

internal UnicodeDecode utf16_decode(uint16_t *str, uint64_t max)
{
    UnicodeDecode result = {1, max_u32};
    result.codepoint = str[0];
    result.inc = 1;
    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000){
        result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
        result.inc = 2;
    }
    return result;
}

internal uint32_t utf8_encode(uint8_t *str, uint32_t codepoint)
{
    uint32_t inc = 0;
    if (codepoint <= 0x7F){
        str[0] = (uint8_t)codepoint;
        inc = 1;
    }
    else if (codepoint <= 0x7FF){
        str[0] = (uint8_t)((bitmask2 << 6)  | ((codepoint >> 6) & bitmask5));
        str[1] = (uint8_t)( bit8            | ( codepoint       & bitmask6));
        inc = 2;
    }
    else if (codepoint <= 0xFFFF){
        str[0] = (uint8_t)((bitmask3 << 5)  | ((codepoint >> 12) & bitmask4));
        str[1] = (uint8_t)( bit8            | ((codepoint >> 6)  & bitmask6));
        str[2] = (uint8_t)( bit8            | ( codepoint        & bitmask6));
        inc = 3;
    }
    else if (codepoint <= 0x10FFFF){
        str[0] = (uint8_t)((bitmask4 << 4)  | ((codepoint >> 18) & bitmask3));
        str[1] = (uint8_t)( bit8            | ((codepoint >> 12) & bitmask6));
        str[2] = (uint8_t)( bit8            | ((codepoint >>  6) & bitmask6));
        str[3] = (uint8_t)( bit8            | ( codepoint        & bitmask6));
        inc = 4;
    }
    else{
        str[0] = '?';
        inc = 1;
    }
    return(inc);
}

internal uint32_t utf16_encode(uint16_t *str, uint32_t codepoint)
{
    uint32_t inc = 1;
    if (codepoint == max_u32){
        str[0] = (uint16_t)'?';
    }
    else if (codepoint < 0x10000){
        str[0] = (uint16_t)codepoint;
    }
    else{
        uint32_t v = codepoint - 0x10000;
        str[0] = safe_cast_u16(0xD800 + (v >> 10));
        str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
        inc = 2;
    }
    return(inc);
}

internal uint32_t utf8_from_utf32_single(uint8_t *buffer, uint32_t character)
{
    return(utf8_encode(buffer, character));
}

// Unicode String Conversions
//=============================================================================

internal Str8 str8_from_16(Alloc alloc, Str16 in)
{
    Str8 result = ZERO_STRUCT;
    if(in.size)
    {
        uint64_t cap = in.size*3;
        uint8_t *str = alloc_make(alloc, uint8_t, cap + 1);
        uint16_t *ptr = in.cstr;
        uint16_t *opl = ptr + in.size;
        uint64_t size = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf16_decode(ptr, opl - ptr);
            size += utf8_encode(str + size, consume.codepoint);
        }
        str[size] = 0;
        // alloc_free(alloc, str, (cap - size));
        result = str8_init(str, size);
    }
    return result;
}

internal Str16 str16_from_8(Alloc alloc, Str8 in)
{
    Str16 result = ZERO_STRUCT;
    if(in.size)
    {
        uint64_t cap = in.size*2;
        uint16_t *str = alloc_make(alloc, uint16_t, cap + 1);
        uint8_t *ptr = in.cstr;
        uint8_t *opl = ptr + in.size;
        uint64_t size = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf8_decode(ptr, opl - ptr);
            size += utf16_encode(str + size, consume.codepoint);
        }
        str[size] = 0;
        // alloc_free(alloc, str, (cap - size)*2);
        result = str16_init(str, size);
    }
    return result;
}

internal Str8 str8_from_32(Alloc alloc, Str32 in)
{
    Str8 result = ZERO_STRUCT;
    if(in.size)
    {
        uint64_t cap = in.size*4;
        uint8_t *str = alloc_make(alloc, uint8_t, cap + 1);
        uint32_t *ptr = in.cstr;
        uint32_t *opl = ptr + in.size;
        uint64_t size = 0;
        for(;ptr < opl; ptr += 1)
        {
            size += utf8_encode(str + size, *ptr);
        }
        str[size] = 0;
        alloc_free(alloc, str, (cap - size));
        result = str8_init(str, size);
    }
    return result;
}

internal Str32 str32_from_8(Alloc alloc, Str8 in)
{
    Str32 result = ZERO_STRUCT;
    if(in.size)
    {
        uint64_t cap = in.size;
        uint32_t *str = alloc_make(alloc, uint32_t, cap + 1);
        uint8_t *ptr = in.cstr;
        uint8_t *opl = ptr + in.size;
        uint64_t size = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf8_decode(ptr, opl - ptr);
            str[size] = consume.codepoint;
            size += 1;
        }
        str[size] = 0;
        alloc_free(alloc, str, (cap - size)*4);
        result = str32_init(str, size);
    }
    return result;
}

// String Hash
//=============================================================================

internal uint64_t str8_hash_u64_from_seed(uint64_t seed, Str8 string)
{
    uint64_t result = XXH3_64bits_withSeed(string.cstr, string.size, seed);
    return result;
}

internal uint64_t str8_hash_u64(Str8 string)
{
    uint64_t result = str8_hash_u64_from_seed(5381, string);
    return result;
}
