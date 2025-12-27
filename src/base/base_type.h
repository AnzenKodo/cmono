#ifndef BASE_TYPE_H
#define BASE_TYPE_H

// External Includes
//=============================================================================

#include <stdint.h>
#include <stddef.h>

// Base Types
//=============================================================================

#if LANG_C
    typedef _Bool bool;
    #define true  1
    #define false 0
#endif
typedef void Void_Proc(void);

// Base Type Array ============================================================

typedef struct U8Array U8Array;
struct U8Array
{
    size_t length;
    size_t size;
    uint8_t  *v;
};
typedef struct U16Array U16Array;
struct U16Array
{
    size_t length;
    size_t size;
    uint16_t *v;
};
typedef struct U32Array U32Array;
struct U32Array
{
    size_t length;
    size_t size;
    uint32_t *v;
};
typedef struct U64Array U64Array;
struct U64Array
{
    size_t length;
    size_t size;
    uint64_t *v;
};

typedef struct I8Array I8Array;
struct I8Array
{
    size_t length;
    size_t size;
    int8_t  *v;
};
typedef struct I16Array I16Array;
struct I16Array
{
    size_t length;
    size_t size;
    int16_t *v;
};
typedef struct I32Array I32Array;
struct I32Array
{
    size_t length;
    size_t size;
    int32_t *v;
};
typedef struct I64Array I64Array;
struct I64Array
{
    size_t length;
    size_t size;
    int64_t *v;
};

typedef struct F32Array F32Array;
struct F32Array
{
    size_t length;
    size_t size;
    float   *v;
};
typedef struct F64Array F64Array;
struct F64Array
{
    size_t length;
    size_t size;
    double  *v;
};

// Macros
//=============================================================================

#define Swap(T,a,b)   do{T t__ = a; a = b; b = t__;}while(0)
#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define TypeOf(T)     __typeof__(T)

#if LANG_CPP
#   define ZERO_STRUCT {}
#else
#   define ZERO_STRUCT {0}
#endif

#define PtrFromInt(i) (void*)((uint8_t*)0 + (i))
#if ARCH_64BIT
#   define IntFromPtr(ptr) ((uint64_t)(ptr))
#elif ARCH_32BIT
#   define IntFromPtr(ptr) ((uint32_t)(ptr))
#else
#   error Missing pointer-to-integer cast for this architecture.
#endif

#define internal        static
#define global          static
#define local_persist   static
#if (COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)) && !(TOOLCHAIN_MINGW)
#   pragma section(".rdata$", read)
#   define read_only __declspec(allocate(".rdata$"))
#elif COMPILER_CLANG && (OS_LINUX || TOOLCHAIN_MINGW)
#   define read_only __attribute__((section(".rodata")))
#else
#   define read_only
#endif

// Array Macros ===============================================================

#define array_alloc(a, T, s) \
    (T){ \
        .size = (s), \
        .length = 0, \
        .v = alloc_make((a), TypeOf(((T){0}).v[0]), (s)) \
    }
#define array_append(array, str) ((array)->v[(array)->length] = (str), &(array)->v[(array)->length++])
#define array_get(array, index) ((index) < (array)->length ? &(array)->v[(index)] : NULL)

// Constants
//=============================================================================

global uint64_t max_u64 = 0xffffffffffffffffull;
global uint32_t max_u32 = 0xffffffff;
global uint16_t max_u16 = 0xffff;
global uint8_t  max_u8  = 0xff;

global uint64_t min_u64 = 0;
global uint32_t min_u32 = 0;
global uint16_t min_u16 = 0;
global uint8_t  min_u8  = 0;

global int64_t max_i64 = (int64_t)0x7fffffffffffffffull;
global int32_t max_i32 = (int32_t)0x7fffffff;
global int16_t max_i16 = (int16_t)0x7fff;
global int8_t  max_i8  =  (int8_t)0x7f;

global int64_t min_i64 = (int64_t)0xffffffffffffffffull;
global int32_t min_i32 = (int32_t)0xffffffff;
global int16_t min_i16 = (int16_t)0xffff;
global int8_t  min_i8  =  (int8_t)0xff;

read_only global uint8_t integer_symbol_reverse[128] =
{
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

// Functions
//=============================================================================

// Safe Casts =================================================================

internal uint16_t safe_cast_u16(uint32_t x);
internal uint32_t safe_cast_u32(uint64_t x);
internal int32_t  safe_cast_i32(int64_t x);
internal uint32_t u32_from_u64_saturate(uint64_t x);
internal int32_t  round_f32_to_i32(float a);
internal uint32_t round_f32_to_u32(float a);

#endif // BASE_TYPE_H
