// String Constructors
//====================================================================

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

// String Matching
//====================================================================

internal bool str8_match(Str8 a, Str8 b)
{
    bool result = 0;
    if(a.size == b.size)
    {
        result = mem_match(a.str, b.str, b.size);
    }
    return result;
}

internal bool str8_ends_with(Str8 string, Str8 end)
{
    Str8 postfix = str8_postfix(string, end.size);
    bool is_match = str8_match(end, postfix);
    return is_match;
}

// String Slicing
//====================================================================

internal Str8 str8_postfix(Str8 str, U64 size)
{
    size = Min(size, str.size);
    str.str = (str.str + str.size) - size;
    str.size = size;
    return(str);
}

internal Str8 str8_cat(Alloc alloc, Str8 s1, Str8 s2)
{
    Str8 str;
    str.size = s1.size + s2.size;
    str.str = alloc_make(alloc, U8, str.size + 1);
    mem_copy(str.str, s1.str, s1.size);
    mem_copy(str.str + s1.size, s2.str, s2.size);
    str.str[str.size] = 0;
    return(str);
}

// C-String Measurement
//====================================================================

internal U64 cstr8_length(U8 *c)
{
    U8 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}
internal U64
cstr16_length(U16 *c){
    U16 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}

internal U64
cstr32_length(U32 *c){
    U32 *p = c;
    for (;*p != 0; p += 1);
    return(p - c);
}

