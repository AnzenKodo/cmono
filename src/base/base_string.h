#ifndef BASE_STRING_H
#define BASE_STRING_H

// External Include
//=============================================================================

#if !defined(XXH_IMPLEMENTATION)
#   define XXH_IMPLEMENTATION
#   define XXH_STATIC_LINKING_ONLY
#   include "../external/xxhash.h"
#endif

// Types
//=============================================================================

// String Types ===============================================================

typedef struct Str8 Str8;
struct Str8 {
    U8 *cstr;
    U64 size;
};
typedef struct Str16 Str16;
struct Str16
{
    U16 *str;
    U64 size;
};
typedef struct Str32 Str32;
struct Str32
{
    U32 *str;
    U64 size;
};

// String List & Array Types ==================================================

typedef struct Str8Node Str8Node;
struct Str8Node
{
  Str8Node *next;
  Str8 string;
};

typedef struct Str8List Str8List;
struct Str8List
{
  Str8Node *first;
  Str8Node *last;
  U64 index;
  U64 size;
};

typedef struct Str8Array Str8Array;
struct Str8Array
{
  Str8 *strings;
  U64 length;
  U64 size;
};

// String Matching, Splitting, & Joining Types ================================

typedef struct StrJoin StrJoin;
struct StrJoin
{
    Str8 pre;
    Str8 sep;
    Str8 post;
};

typedef enum StrSplitFlags
{
  StrSplitFlag_KeepEmpties = (1 << 0),
} StrSplitFlags;

typedef enum StrMatchFlags
{
    StrMatchFlag_CaseInsensitive  = (1 << 0),
    StrMatchFlag_RightSideSloppy  = (1 << 1),
    StrMatchFlag_SlashInsensitive = (1 << 2),
} StrMatchFlags;

// Character Classification & Conversion Functions ============================

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
    U32 inc;
    U32 codepoint;
};

// Functions
//=============================================================================

// Character Classification & Conversion Functions ============================

internal bool char_is_space(U8 c);
internal bool char_is_upper(U8 c);
internal bool char_is_lower(U8 c);
internal bool char_is_alpha(U8 c);
internal bool char_is_slash(U8 c);
internal U8   char_to_lower(U8 c);
internal U8   char_to_upper(U8 c);
internal U8   char_to_correct_slash(U8 c);

// C-String Measurement =======================================================

internal U64 cstr8_length(U8 *c);
internal U64 cstr16_length(U16 *c);
internal U64 cstr32_length(U32 *c);

// String Constructors ========================================================

#define str8(S)  str8_init((U8*)(S), sizeof(S) - 1)
internal Str8 str8_init(U8 *str, U64 size);
internal Str8 str8_from_cstr(char *c);

internal Str16 str16_init(U16 *str, U64 size);
internal Str16 str16_from_cstr(U16 *c);

internal Str32 str32_init(U32 *str, U64 size);
internal Str32 str32_from_cstr(U32 *c);

// String Matching ============================================================

internal bool str8_match(Str8 a, Str8 b, StrMatchFlags flags);
internal bool str8_ends_with(Str8 string, Str8 end);
internal U64 str8_find_substr(Str8 string, U64 start_pos, Str8 substr, StrMatchFlags flags);
internal U64 str8_find_substr_reverse(Str8 string, U64 start_pos, Str8 substr, StrMatchFlags flags);

// String Slicing =============================================================

internal Str8 str8_substr(Str8 str, Rng1U64 range);
internal Str8 str8_postfix(Str8 str, U64 size);
internal Str8 str8_prefix(Str8 str, U64 size);
internal Str8 str8_skip(Str8 str, U64 amt);
internal Str8 str8_cat(Alloc alloc, Str8 s1, Str8 s2);

// String List Construction Functions =========================================

internal Str8Node* str8_list_push(Alloc alloc, Str8List *list, Str8 string);

// String Arrays ==============================================================

internal Str8Array str8_array_from_list(Alloc alloc, Str8List *list);
internal Str8Array str8_array_reserve(Alloc alloc, U64 count);

// String Split and Join ======================================================

internal Str8List str8_split(Alloc alloc, Str8 string, U8 *split_chars, U64 split_char_count, StrSplitFlags flags);
internal Str8 str8_list_join(Alloc alloc, Str8List *list, StrJoin *optional_params);

// String Formatting & Copying ================================================

internal Str8 str8_copy(Alloc alloc, Str8 s);

// UTF-8 & UTF-16 Decoding/Encoding ===========================================

internal UnicodeDecode utf8_decode(U8 *str, U64 max);
internal UnicodeDecode utf16_decode(U16 *str, U64 max);
internal U32 utf8_encode(U8 *str, U32 codepoint);
internal U32 utf16_encode(U16 *str, U32 codepoint);
internal U32 utf8_from_utf32_single(U8 *buffer, U32 character);

// Unicode String Conversions =================================================

internal Str8 str8_from_16(Alloc alloc, Str16 in);
internal Str16 str16_from_8(Alloc alloc, Str8 in);
internal Str8 str8_from_32(Alloc alloc, Str32 in);
internal Str32 str32_from_8(Alloc alloc, Str8 in);

// String Hash ================================================================

internal U64 str8_hash_u64_from_seed(U64 seed, Str8 string);
internal U64 str8_hash_u64(Str8 string);

#endif // BASE_STRING_H
