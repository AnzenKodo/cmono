// Character Classification & Conversion Functions
//=============================================================================

internal bool char_is_space(uint8_t c)
{
    return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
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

internal uint8_t char_to_correct_slash(uint8_t c)
{
    if(char_is_slash(c))
    {
        c = '/';
    }
    return(c);
}

// C-String Measurement
//=============================================================================

internal size_t cstr8_length(uint8_t *cstr)
{
    uint8_t *p = cstr;
    for (;*p != 0; p += 1);
    return(p - cstr);
}
internal size_t cstr16_length(uint16_t *cstr)
{
    uint16_t *p = cstr;
    for (;*p != 0; p += 1);
    return(p - cstr);
}
internal size_t cstr32_length(uint32_t *cstr)
{
    uint32_t *p = cstr;
    for (;*p != 0; p += 1);
    return(p - cstr);
}

// String Constructors
//=============================================================================

internal Str8 str8_init(uint8_t *cstr, size_t size)
{
    return (Str8){cstr, size, size};
}
internal Str8 str8_from_cstr(char *cstr)
{
    size_t len = cstr8_length((uint8_t*)cstr);
    return (Str8){(uint8_t*)cstr, len, len};
}

internal Str16 str16_init(uint16_t *cstr, size_t size)
{
    return (Str16){cstr, size, size};
}
internal Str16 str16_from_cstr(uint16_t *cstr)
{
    size_t len = cstr16_length((uint16_t*)cstr);
    return (Str16){(uint16_t*)cstr, len, len};
}

internal Str32 str32_init(uint32_t *cstr, size_t size)
{
    return (Str32){cstr, size, size};
}
internal Str32 str32_from_cstr(uint32_t *cstr)
{
    size_t len = cstr32_length((uint32_t*)cstr);
    return (Str32){(uint32_t*)cstr, len, len};
}

internal Str8 str8_range(uint8_t *first, uint8_t *one_past_last, Str8 str)
{
    return (Str8){first, (size_t)(one_past_last - first), str.size};
}

// String Matching
//=============================================================================

internal bool str8_match(Str8 a, Str8 b, StrMatchFlags flags)
{
    bool result = 0;
    if (a.length == b.length && flags == 0)
    {
        result = mem_match(a.cstr, b.cstr, b.length);
    }
    else if (a.length == b.length || (flags & StrMatchFlag_RightSideSloppy))
    {
        bool case_insensitive  = (flags & StrMatchFlag_CaseInsensitive);
        bool slash_insensitive = (flags & StrMatchFlag_SlashInsensitive);
        size_t length               = Min(a.length, b.length);
        result = 1;
        for(size_t i = 0; i < length; i += 1)
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

internal bool str8_ends_with(Str8 str, Str8 end)
{
    Str8 postfix = str8_postfix(str, end.length);
    bool is_match = str8_match(end, postfix, 0);
    return is_match;
}

internal size_t str8_find_substr(Str8 str, size_t start_pos, Str8 substr, StrMatchFlags flags)
{
    uint8_t *p = str.cstr + start_pos;
    size_t stop_offset = Max(str.length + 1, substr.length) - substr.length;
    uint8_t *stop_p = str.cstr + stop_offset;
    if (substr.length > 0)
    {
        uint8_t *string_opl = str.cstr + str.length;
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
                if (str8_match(str8_range(p+1, string_opl, str), needle_tail, adjusted_flags))
                {
                    break;
                }
            }
        }
    }
    size_t result = str.length;
    if (p < stop_p)
    {
        result = (size_t)(p - str.cstr);
    }
    return result;
}

internal size_t str8_find_substr_reverse(Str8 str, size_t start_pos, Str8 substr, StrMatchFlags flags)
{
    size_t result = 0;
    for(int64_t i = str.length - start_pos - substr.length; i >= 0; --i)
    {
        Str8 haystack = str8_substr(str, rng1_u64(i, i + substr.length));
        if(str8_match(haystack, substr, flags))
        {
            result = (size_t)i + substr.length;
            break;
        }
    }
    return result;
}

// String Slicing
//=============================================================================

internal Str8 str8_substr(Str8 str, Rng1_U64 range)
{
    range.min = Min(range.min, str.length);
    range.max = Min(range.max, str.length);
    str.cstr += range.min;
    str.length = dim1_u64(range);
    return(str);
}

internal Str8 str8_postfix(Str8 str, size_t length)
{
    length = Min(length, str.length);
    str.cstr = (str.cstr + str.length) - length;
    str.length = length;
    return(str);
}

internal Str8 str8_prefix(Str8 str, size_t length)
{
    str.length = Min(length, str.length);
    return str;
}

internal Str8 str8_skip(Str8 str, size_t amt)
{
    amt = Min(amt, str.length);
    str.cstr += amt;
    str.length -= amt;
    return str;
}

internal Str8 str8_cat(Alloc alloc, Str8 s1, Str8 s2)
{
    Str8 str;
    str.length = s1.length + s2.length;
    str.cstr = alloc_make(alloc, uint8_t, str.length + 1);
    mem_copy(str.cstr, s1.cstr, s1.length);
    mem_copy(str.cstr + s1.length, s2.cstr, s2.length);
    str.cstr[str.length] = 0;
    return(str);
}

// String Conversions
//=============================================================================

internal int64_t str8_to_sign(Str8 str, Str8 *string_tail)
{
    // count negative signs
    size_t neg_count = 0;
    size_t i = 0;
    for(; i < str.length; i += 1)
    {
        if (str.cstr[i] == '-'){
            neg_count += 1;
        }
        else if (str.cstr[i] != '+'){
            break;
        }
    }

    // output part of string after signs
    *string_tail = str8_skip(str, i);

    // output integer sign
    int64_t sign = (neg_count & 1)?-1:+1;
    return sign;
}

internal bool str8_is_integer(Str8 str, uint32_t radix)
{
  bool result = false;
  Str8 sign = str8_prefix(str, 1);
  if(str8_match(sign, str8("-"), 0))
  {
    result = str8_is_integer_unsigned(str8_skip(str, 1), radix);
  }
  else
  {
    result = str8_is_integer_unsigned(str, radix);
  }
  return result;
}


internal bool str8_is_integer_unsigned(Str8 str, uint32_t radix)
{
    bool result = false;
    if(str.length > 0)
    {
        if(1 < radix && radix <= 16)
        {
            result = true;
            for(size_t i = 0; i < str.length; i += 1)
            {
                uint8_t c = str.cstr[i];
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

internal bool str8_is_float(Str8 str)
{
    if (str.length == 0) {
        return false;
    }
    size_t i = 0;
    if (i < str.length && (str.cstr[i] == '+' || str.cstr[i] == '-'))
    {
        i += 1;
    }
    bool has_digits = false;
    bool has_dot = false;
    bool has_exp = false;
    while (i < str.length)
    {
        uint8_t c = str.cstr[i];
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
            if (i >= str.length)
            {
                return false; // 'e' at end is invalid
            }
            // Optional exponent sign
            if (str.cstr[i] == '+' || str.cstr[i] == '-')
            {
                i += 1;
            }
            // Must have at least one digit in exponent
            if (i >= str.length || str.cstr[i] < '0' || str.cstr[i] > '9')
            {
                return false;
            }
            // Skip remaining exponent digits
            while (i < str.length && str.cstr[i] >= '0' && str.cstr[i] <= '9')
            {
                i += 1;
            }
            return i == str.length; // Must consume all after 'e'
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

internal uint64_t str8_to_u64(Str8 str, uint32_t radix)
{
    uint64_t x = 0;
    if(1 < radix && radix <= 16)
    {
        for(uint64_t i = 0; i < str.length; i += 1)
        {
            x *= radix;
            x += integer_symbol_reverse[str.cstr[i]&0x7F];
        }
    }
    return x;
}

internal uint32_t str8_to_u32(Str8 str, uint32_t radix)
{
    uint64_t x64 = str8_to_u64(str, radix);
    uint32_t x32 = safe_cast_u32(x64);
    return x32;
}

internal int64_t str8_to_i64(Str8 str, uint32_t radix)
{
    int64_t sign = str8_to_sign(str, &str);
    int64_t x = (int64_t)str8_to_u64(str, radix) * sign;
    return x;
}

internal int32_t str8_to_i32(Str8 str, uint32_t radix)
{
    int64_t x64 = str8_to_i64(str, radix);
    int32_t x32 = safe_cast_s32(x64);
    return x32;
}

internal double str8_to_f64(Str8 str)
{
    // TODO(rjf): crappy implementation for now that just uses atof.
    double result = 0;
    if(str.length > 0)
    {
        // Find starting pos of numeric string, as well as sign
        double sign = +1.0;
        if(str.cstr[0] == '-')
        {
            sign = -1.0;
        }
        else if(str.cstr[0] == '+')
        {
            sign = 1.0;
        }
        // Gather numerics
        size_t num_valid_chars = 0;
        char buffer[64];
        bool exp = 0;
        for(size_t idx = 0; idx < str.length && num_valid_chars < sizeof(buffer)-1; idx += 1)
        {
            if(char_is_digit(str.cstr[idx], 10) || str.cstr[idx] == '.' || str.cstr[idx] == 'e' ||
                    (exp && (str.cstr[idx] == '+' || str.cstr[idx] == '-')))
            {
                buffer[num_valid_chars] = str.cstr[idx];
                num_valid_chars += 1;
                exp = 0;
                exp = (str.cstr[idx] == 'e');
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

internal Str8Node* str8_list_push(Alloc alloc, Str8List *list, Str8 str)
{
    Str8Node *node = alloc_make(alloc, Str8Node, 1);
    SLLQueuePush(list->first, list->last, node);
    list->length += 1;
    list->capacity += str.length;
    node->str = str;
    return node;
}

// String Arrays Construction Functions
// ==============================================================

internal Str8Array str8_array_from_list(Alloc alloc, Str8List *list)
{
    Str8Array array;
    array.size = list->length;
    array.length = array.size;
    array.v = alloc_make(alloc, Str8, array.size);
    size_t idx = 0;
    for (Str8Node *n = list->first; n != 0; n = n->next, idx += 1)
    {
        array.v[idx] = n->str;
    }
    return array;
}

// String Split and Join
//=============================================================================

internal Str8List str8_split(Alloc alloc, Str8 str, uint8_t *split_chars, size_t split_char_count, StrSplitFlags flags)
{
    Str8List list = ZERO_STRUCT;
    bool keep_empties = (flags & StrSplitFlag_KeepEmpties);
    uint8_t *ptr = str.cstr;
    uint8_t *opl = str.cstr + str.length;
    for (;ptr < opl;)
    {
        uint8_t *first = ptr;
        for (;ptr < opl; ptr += 1)
        {
            uint8_t c = *ptr;
            bool is_split = false;
            for (size_t i = 0; i < split_char_count; i += 1)
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
        Str8 node = str8_range(first, ptr, str);
        if (keep_empties || node.length > 0)
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
    size_t sep_count = 0;
    if (list->length > 0)
    {
        sep_count = list->length - 1;
    }
    Str8 result;
    result.length = join.pre.length + join.post.length + sep_count*join.sep.length + list->capacity;
    uint8_t *ptr = result.cstr = alloc_make(alloc, uint8_t, result.length + 1);
    mem_copy(ptr, join.pre.cstr, join.pre.length);
    ptr += join.pre.length;
    for (Str8Node *node = list->first; node != 0; node = node->next)
    {
        mem_copy(ptr, node->str.cstr, node->str.length);
        ptr += node->str.length;
        if (node->next != 0)
        {
            mem_copy(ptr, join.sep.cstr, join.sep.length);
            ptr += join.sep.length;
        }
    }
    mem_copy(ptr, join.post.cstr, join.post.length);
    ptr += join.post.length;
    *ptr = 0;
    return result;
}

// String Formatting & Copying
//=============================================================================

internal Str8 str8_copy(Alloc alloc, Str8 s)
{
    Str8 str;
    str.length = s.length;
    str.cstr = alloc_make(alloc, uint8_t, str.length + 1);
    mem_copy(str.cstr, s.cstr, s.length);
    str.cstr[str.length] = 0;
    return(str);
}

internal Str8 str8fv(Alloc alloc, char *fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);
    uint32_t needed_bytes = fmt_vsnprintf(0, 0, fmt, args) + 1;
    Str8 result = ZERO_STRUCT;
    result.cstr = alloc_make(alloc, uint8_t, needed_bytes);
    result.length = fmt_vsnprintf((char*)result.cstr, needed_bytes, fmt, args2);
    result.cstr[result.length] = 0;
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

internal UnicodeDecode utf8_decode(uint8_t *str, size_t max)
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

internal UnicodeDecode utf16_decode(uint16_t *str, size_t max)
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
    if(in.length)
    {
        size_t cap = in.length*3;
        uint8_t *str = alloc_make(alloc, uint8_t, cap + 1);
        uint16_t *ptr = in.cstr;
        uint16_t *opl = ptr + in.length;
        size_t length = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf16_decode(ptr, opl - ptr);
            length += utf8_encode(str + length, consume.codepoint);
        }
        str[length] = 0;
        // alloc_free(alloc, str, (cap - length));
        result = str8_init(str, length);
    }
    return result;
}

internal Str16 str16_from_8(Alloc alloc, Str8 in)
{
    Str16 result = ZERO_STRUCT;
    if(in.length)
    {
        size_t cap = in.length*2;
        uint16_t *str = alloc_make(alloc, uint16_t, cap + 1);
        uint8_t *ptr = in.cstr;
        uint8_t *opl = ptr + in.length;
        size_t length = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf8_decode(ptr, opl - ptr);
            length += utf16_encode(str + length, consume.codepoint);
        }
        str[length] = 0;
        // alloc_free(alloc, str, (cap - length)*2);
        result = str16_init(str, length);
    }
    return result;
}

internal Str8 str8_from_32(Alloc alloc, Str32 in)
{
    Str8 result = ZERO_STRUCT;
    if(in.length)
    {
        size_t cap = in.length*4;
        uint8_t *str = alloc_make(alloc, uint8_t, cap + 1);
        uint32_t *ptr = in.cstr;
        uint32_t *opl = ptr + in.length;
        size_t length = 0;
        for(;ptr < opl; ptr += 1)
        {
            length += utf8_encode(str + length, *ptr);
        }
        str[length] = 0;
        alloc_free(alloc, str, (cap - length));
        result = str8_init(str, length);
    }
    return result;
}

internal Str32 str32_from_8(Alloc alloc, Str8 in)
{
    Str32 result = ZERO_STRUCT;
    if(in.length)
    {
        size_t cap = in.length;
        uint32_t *str = alloc_make(alloc, uint32_t, cap + 1);
        uint8_t *ptr = in.cstr;
        uint8_t *opl = ptr + in.length;
        size_t length = 0;
        UnicodeDecode consume;
        for(;ptr < opl; ptr += consume.inc)
        {
            consume = utf8_decode(ptr, opl - ptr);
            str[length] = consume.codepoint;
            length += 1;
        }
        str[length] = 0;
        alloc_free(alloc, str, (cap - length)*4);
        result = str32_init(str, length);
    }
    return result;
}

// String Hash
//=============================================================================

internal uint64_t str8_hash_u64_from_seed(uint64_t seed, Str8 str)
{
    uint64_t result = XXH3_64bits_withSeed(str.cstr, str.length, seed);
    return result;
}

internal uint64_t str8_hash_u64(Str8 str)
{
    uint64_t result = str8_hash_u64_from_seed(5381, str);
    return result;
}
