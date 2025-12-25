#ifndef BASE_STRING_H
#define BASE_STRING_H

// External Include
//=============================================================================

#if !defined(XXH_IMPLEMENTATION)
#   define XXH_INLINE_ALL
#   define XXH_IMPLEMENTATION
#   define XXH_STATIC_LINKING_ONLY
#   include "../external/xxhash.h"
#endif

// Types
//=============================================================================

// String Types ===============================================================

typedef struct Str8 Str8;
struct Str8 {
    uint8_t *cstr;
    size_t length;
    size_t size;
};
typedef struct Str16 Str16;
struct Str16
{
    uint16_t *cstr;
    size_t length;
    size_t size;
};
typedef struct Str32 Str32;
struct Str32
{
    uint32_t *cstr;
    size_t length;
    size_t size;
};

// String List & Array Types ==================================================

typedef struct Str8Node Str8Node;
struct Str8Node
{
  Str8Node *next;
  Str8 str;
};

typedef struct Str8List Str8List;
struct Str8List
{
  Str8Node *first;
  Str8Node *last;
  size_t length;
  size_t capacity;
};

typedef struct Str8Array Str8Array;
struct Str8Array
{
  Str8 *v;
  size_t length;
  size_t size;
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
    uint32_t inc;
    uint32_t codepoint;
};

// Functions
//=============================================================================

// Character Classification & Conversion Functions ============================

internal bool    char_is_space(uint8_t c);
internal bool    char_is_upper(uint8_t c);
internal bool    char_is_lower(uint8_t c);
internal bool    char_is_alpha(uint8_t c);
internal bool    char_is_digit(uint8_t c, uint32_t base);
internal bool    char_is_slash(uint8_t c);
internal uint8_t char_to_lower(uint8_t c);
internal uint8_t char_to_upper(uint8_t c);
internal uint8_t char_to_correct_slash(uint8_t c);

// C-String Measurement =======================================================

internal size_t cstr8_length(uint8_t *c);
internal size_t cstr16_length(uint16_t *c);
internal size_t cstr32_length(uint32_t *c);

// String Constructors ========================================================

#define str8(S)      str8_init((uint8_t*)(S), sizeof(S) - 1)
#define str8_varg(S) (int)((S).length), ((S).cstr)
internal Str8  str8_init(uint8_t *str, size_t size);
internal Str8  str8_from_cstr(char *c);
internal Str16 str16_init(uint16_t *str, size_t size);
internal Str16 str16_from_cstr(uint16_t *c);
internal Str32 str32_init(uint32_t *str, size_t size);
internal Str32 str32_from_cstr(uint32_t *c);
internal Str8  str8_range(uint8_t *first, uint8_t *one_past_last, Str8 str);

// String Matching ============================================================

internal bool   str8_match(Str8 a, Str8 b, StrMatchFlags flags);
internal bool   str8_ends_with(Str8 str, Str8 end);
internal size_t str8_find_substr(Str8 str, size_t start_pos, Str8 substr, StrMatchFlags flags);
internal size_t str8_find_substr_reverse(Str8 str, size_t start_pos, Str8 substr, StrMatchFlags flags);

// String Slicing =============================================================

internal Str8 str8_substr(Str8 str, Rng1_U64 range);
internal Str8 str8_postfix(Str8 str, size_t length);
internal Str8 str8_prefix(Str8 str, size_t length);
internal Str8 str8_skip(Str8 str, size_t amt);
internal Str8 str8_cat(Alloc alloc, Str8 s1, Str8 s2);

// String Conversions =========================================================

internal int64_t  str8_to_sign(Str8 str, Str8 *string_tail);
internal bool     str8_is_integer(Str8 str, uint32_t radix);
internal bool     str8_is_integer_unsigned(Str8 str, uint32_t radix);
internal bool     str8_is_float(Str8 str);
internal uint64_t str8_to_u64(Str8 str, uint32_t radix);
internal uint32_t str8_to_u32(Str8 str, uint32_t radix);
internal int64_t  str8_to_i64(Str8 str, uint32_t radix);
internal int32_t  str8_to_i32(Str8 str, uint32_t radix);
internal double   str8_to_f64(Str8 str);
internal Str8     str8_from_bool(bool value);
internal bool     str8_is_bool(Str8 str);
internal bool     str8_to_bool(Str8 str);

// String List Construction Functions =========================================

internal Str8Node* str8_list_push(Alloc alloc, Str8List *list, Str8 str);

// String Arrays Construction Functions =======================================

internal Str8Array str8_array_from_list(Alloc alloc, Str8List *list);

// String Split and Join ======================================================

internal Str8List str8_split(Alloc alloc, Str8 str, uint8_t *split_chars, size_t split_char_count, StrSplitFlags flags);
internal Str8     str8_list_join(Alloc alloc, Str8List *list, StrJoin *optional_params);

// String Formatting & Copying ================================================

internal Str8 str8_copy(Alloc alloc, Str8 s);
internal Str8 str8fv(Alloc alloc, char *fmt, va_list args);
internal Str8 str8f(Alloc alloc, char *fmt, ...);

// UTF-8 & UTF-16 Decoding/Encoding ===========================================

internal UnicodeDecode utf8_decode(uint8_t *str, size_t max);
internal UnicodeDecode utf16_decode(uint16_t *str, size_t max);
internal uint32_t      utf8_encode(uint8_t *str, uint32_t codepoint);
internal uint32_t      utf16_encode(uint16_t *str, uint32_t codepoint);
internal uint32_t      utf8_from_utf32_single(uint8_t *buffer, uint32_t character);

// Unicode String Conversions =================================================

internal Str8  str8_from_16(Alloc alloc, Str16 in);
internal Str16 str16_from_8(Alloc alloc, Str8 in);
internal Str8  str8_from_32(Alloc alloc, Str32 in);
internal Str32 str32_from_8(Alloc alloc, Str8 in);

// String Hash ================================================================

internal uint64_t str8_hash_u64_from_seed(uint64_t seed, Str8 str);
internal uint64_t str8_hash_u64(Str8 str);

// Globals
//=============================================================================

read_only global uint8_t utf8_class[32] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

#endif // BASE_STRING_H
