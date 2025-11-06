#ifndef BASE_CORE_H
#define BASE_CORE_H

// External Includes
//=============================================================================

#include <stdint.h>
#include <stdarg.h>

// Base Types
//=============================================================================

#if LANG_C
    typedef _Bool bool;
    #define true  1
    #define false 0
#endif

typedef uint8_t   U8;
typedef int8_t    I8;
typedef uint16_t  U16;
typedef int16_t   I16;
typedef uint32_t  U32;
typedef int32_t   I32;
typedef uint64_t  U64;
typedef int64_t   I64;
typedef float     F32;
typedef double    F64;
typedef void Void_Proc(void);

// Toolchain/Environment Enums
//=============================================================================

typedef enum Context_Os
{
    Context_Os_Null,
    Context_Os_Windows,
    Context_Os_Linux,
    Context_Os_Mac,
    Context_Os_COUNT,
}
Context_Os;

typedef enum Context_Arch
{
    Context_Arch_Null,
    Context_Arch_X64,
    Context_Arch_X86,
    Context_Arch_Arm64,
    Context_Arch_Arm32,
    Context_Arch_COUNT,
}
Context_Arch;

typedef enum Context_Compiler
{
    Context_Compiler_Null,
    Context_Compiler_msvc,
    Context_Compiler_gcc,
    Context_Compiler_clang,
    Context_Compiler_COUNT,
}
Context_Compiler;

// Code Keywords
//=============================================================================

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

#if LANG_CPP
#   define ZERO_STRUCT {}
#else
#   define ZERO_STRUCT {0}
#endif

#if COMPILER_CLANG || COMPILER_GCC
#   define FILE_NAME __FILE_NAME__
#else
#   define FILE_NAME __FILE__
#endif
#define LINE_NUMBER __LINE__

// Constants
//=============================================================================

global U64 max_u64 = 0xffffffffffffffffull;
global U32 max_u32 = 0xffffffff;
global U16 max_u16 = 0xffff;
global U8  max_u8  = 0xff;

global I64 max_i64 = (I64)0x7fffffffffffffffull;
global I32 max_i32 = (I32)0x7fffffff;
global I16 max_i16 = (I16)0x7fff;
global I8  max_i8  =  (I8)0x7f;

global I64 min_i64 = (I64)0xffffffffffffffffull;
global I32 min_i32 = (I32)0xffffffff;
global I16 min_i16 = (I16)0xffff;
global I8  min_i8  =  (I8)0xff;

global const U32 bitmask1  = 0x00000001;
global const U32 bitmask2  = 0x00000003;
global const U32 bitmask3  = 0x00000007;
global const U32 bitmask4  = 0x0000000f;
global const U32 bitmask5  = 0x0000001f;
global const U32 bitmask6  = 0x0000003f;
global const U32 bitmask7  = 0x0000007f;
global const U32 bitmask8  = 0x000000ff;
global const U32 bitmask9  = 0x000001ff;
global const U32 bitmask10 = 0x000003ff;
global const U32 bitmask11 = 0x000007ff;
global const U32 bitmask12 = 0x00000fff;
global const U32 bitmask13 = 0x00001fff;
global const U32 bitmask14 = 0x00003fff;
global const U32 bitmask15 = 0x00007fff;
global const U32 bitmask16 = 0x0000ffff;
global const U32 bitmask17 = 0x0001ffff;
global const U32 bitmask18 = 0x0003ffff;
global const U32 bitmask19 = 0x0007ffff;
global const U32 bitmask20 = 0x000fffff;
global const U32 bitmask21 = 0x001fffff;
global const U32 bitmask22 = 0x003fffff;
global const U32 bitmask23 = 0x007fffff;
global const U32 bitmask24 = 0x00ffffff;
global const U32 bitmask25 = 0x01ffffff;
global const U32 bitmask26 = 0x03ffffff;
global const U32 bitmask27 = 0x07ffffff;
global const U32 bitmask28 = 0x0fffffff;
global const U32 bitmask29 = 0x1fffffff;
global const U32 bitmask30 = 0x3fffffff;
global const U32 bitmask31 = 0x7fffffff;
global const U32 bitmask32 = 0xffffffff;
global const U64 bitmask33 = 0x00000001ffffffffull;
global const U64 bitmask34 = 0x00000003ffffffffull;
global const U64 bitmask35 = 0x00000007ffffffffull;
global const U64 bitmask36 = 0x0000000fffffffffull;
global const U64 bitmask37 = 0x0000001fffffffffull;
global const U64 bitmask38 = 0x0000003fffffffffull;
global const U64 bitmask39 = 0x0000007fffffffffull;
global const U64 bitmask40 = 0x000000ffffffffffull;
global const U64 bitmask41 = 0x000001ffffffffffull;
global const U64 bitmask42 = 0x000003ffffffffffull;
global const U64 bitmask43 = 0x000007ffffffffffull;
global const U64 bitmask44 = 0x00000fffffffffffull;
global const U64 bitmask45 = 0x00001fffffffffffull;
global const U64 bitmask46 = 0x00003fffffffffffull;
global const U64 bitmask47 = 0x00007fffffffffffull;
global const U64 bitmask48 = 0x0000ffffffffffffull;
global const U64 bitmask49 = 0x0001ffffffffffffull;
global const U64 bitmask50 = 0x0003ffffffffffffull;
global const U64 bitmask51 = 0x0007ffffffffffffull;
global const U64 bitmask52 = 0x000fffffffffffffull;
global const U64 bitmask53 = 0x001fffffffffffffull;
global const U64 bitmask54 = 0x003fffffffffffffull;
global const U64 bitmask55 = 0x007fffffffffffffull;
global const U64 bitmask56 = 0x00ffffffffffffffull;
global const U64 bitmask57 = 0x01ffffffffffffffull;
global const U64 bitmask58 = 0x03ffffffffffffffull;
global const U64 bitmask59 = 0x07ffffffffffffffull;
global const U64 bitmask60 = 0x0fffffffffffffffull;
global const U64 bitmask61 = 0x1fffffffffffffffull;
global const U64 bitmask62 = 0x3fffffffffffffffull;
global const U64 bitmask63 = 0x7fffffffffffffffull;
global const U64 bitmask64 = 0xffffffffffffffffull;

global const U32 bit1  = (1<<0);
global const U32 bit2  = (1<<1);
global const U32 bit3  = (1<<2);
global const U32 bit4  = (1<<3);
global const U32 bit5  = (1<<4);
global const U32 bit6  = (1<<5);
global const U32 bit7  = (1<<6);
global const U32 bit8  = (1<<7);
global const U32 bit9  = (1<<8);
global const U32 bit10 = (1<<9);
global const U32 bit11 = (1<<10);
global const U32 bit12 = (1<<11);
global const U32 bit13 = (1<<12);
global const U32 bit14 = (1<<13);
global const U32 bit15 = (1<<14);
global const U32 bit16 = (1<<15);
global const U32 bit17 = (1<<16);
global const U32 bit18 = (1<<17);
global const U32 bit19 = (1<<18);
global const U32 bit20 = (1<<19);
global const U32 bit21 = (1<<20);
global const U32 bit22 = (1<<21);
global const U32 bit23 = (1<<22);
global const U32 bit24 = (1<<23);
global const U32 bit25 = (1<<24);
global const U32 bit26 = (1<<25);
global const U32 bit27 = (1<<26);
global const U32 bit28 = (1<<27);
global const U32 bit29 = (1<<28);
global const U32 bit30 = (1<<29);
global const U32 bit31 = (1<<30);
global const U32 bit32 = (1<<31);
global const U64 bit33 = (1ull<<32);
global const U64 bit34 = (1ull<<33);
global const U64 bit35 = (1ull<<34);
global const U64 bit36 = (1ull<<35);
global const U64 bit37 = (1ull<<36);
global const U64 bit38 = (1ull<<37);
global const U64 bit39 = (1ull<<38);
global const U64 bit40 = (1ull<<39);
global const U64 bit41 = (1ull<<40);
global const U64 bit42 = (1ull<<41);
global const U64 bit43 = (1ull<<42);
global const U64 bit44 = (1ull<<43);
global const U64 bit45 = (1ull<<44);
global const U64 bit46 = (1ull<<45);
global const U64 bit47 = (1ull<<46);
global const U64 bit48 = (1ull<<47);
global const U64 bit49 = (1ull<<48);
global const U64 bit50 = (1ull<<49);
global const U64 bit51 = (1ull<<50);
global const U64 bit52 = (1ull<<51);
global const U64 bit53 = (1ull<<52);
global const U64 bit54 = (1ull<<53);
global const U64 bit55 = (1ull<<54);
global const U64 bit56 = (1ull<<55);
global const U64 bit57 = (1ull<<56);
global const U64 bit58 = (1ull<<57);
global const U64 bit59 = (1ull<<58);
global const U64 bit60 = (1ull<<59);
global const U64 bit61 = (1ull<<60);
global const U64 bit62 = (1ull<<61);
global const U64 bit63 = (1ull<<62);
global const U64 bit64 = (1ull<<63);

// Link Lists
//=============================================================================

#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

#define SLLPush_NZ(nil,fist,last,node,next) (CheckNil(nil,fist)?\
    ((fist)=(last)=(node),SetNil(nil,(node)->next)):\
    ((last)->next=(node),(last)=(node),SetNil(nil,(node)->next)))
#define SLLPushFront_NZ(nil,first,last,node,next) (CheckNil(nil,first)?\
    ((first)=(last)=(node),SetNil(nil,(node)->next)):\
    ((node)->next=(first),(first)=(node)))
#define SLLPop_NZ(nil,first,last,next) ((first)==(last)?\
    (SetNil(nil,first),SetNil(nil,last)):\
    ((first)=(first)->next))

#define SLLPush(first,last,node) SLLPush_NZ(0,first,last,node,next)
#define SLLPushFront(fist,last,node) SLLPushFront_NZ(0,fist,last,node,next)
#define SLLPop(first,last) SLLPop_NZ(0,first,last,next)

// Misc. Macros
//=============================================================================

#define Unused(var)   (void)var
#define Cast(Type)    (Type)
#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define Swap(T,a,b)   do{T t__ = a; a = b; b = t__;}while(0)

#define Min(A,B)     (((A)<(B))?(A):(B))
#define Max(A,B)     (((A)>(B))?(A):(B))
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

#define IsPow2(x)       ((x != 0) && ((x & (x - 1)) == 0))
#define AlignPow2(x,b)  (((x) + (b) - 1)&(~((b) - 1)))

#if ARCH_64BIT
#   define IntFromPtr(ptr) ((U64)(ptr))
#elif ARCH_32BIT
#   define IntFromPtr(ptr) ((U32)(ptr))
#else
#   error Missing pointer-to-integer cast for this architecture.
#endif
#define PtrFromInt(i) (void*)((U8*)0 + (i))

// Alignment ==================================================================

#if COMPILER_MSVC
#   define AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
#   define AlignOf(T) __alignof(T)
#elif COMPILER_GCC
#   define AlignOf(T) __alignof__(T)
#else
#   error AlignOf not defined for this compiler.
#endif

// Asserts ====================================================================

#if COMPILER_MSVC
#   define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
#   define Trap() __builtin_trap()
#elif COMPILER_TCC
#   define Trap() asm volatile("ud2");
#else
#   error Unknown trap intrinsic for this compiler.
#endif

#define AssertAlways(x) do{if(!(x)) {Trap();}}while(0)
#if BUILD_DEBUG
#   define Assert(x) AssertAlways(x)
#else
#   define Assert(x) (void)(x)
#endif
#define InvalidCodePath Assert(!"Invalid Path!")
#define NotImplemented  Assert(!"Not Implemented!")

// Defines
//=============================================================================

// ANSI Codes =================================================================

/* Reset */
#define ANSI_RESET             "\x1B[0m"
#define ANSI_BOLD              "\x1B[1m"
#define ANSI_FAINT             "\x1B[2m"
#define ANSI_ITALIC            "\x1B[3m"
#define ANSI_UNDERLINE         "\x1B[4m"
#define ANSI_BLINK_SLOW        "\x1B[5m"
#define ANSI_BLINK_FAST        "\x1B[6m"
#define ANSI_REVERSE           "\x1B[7m"
#define ANSI_CONCEAL           "\x1B[8m"
#define ANSI_STRIKETHROUGH     "\x1B[9m"

/* Foreground */
#define ANSI_FG_BLACK          "\x1B[30m"
#define ANSI_FG_RED            "\x1B[31m"
#define ANSI_FG_GREEN          "\x1B[32m"
#define ANSI_FG_YELLOW         "\x1B[33m"
#define ANSI_FG_BLUE           "\x1B[34m"
#define ANSI_FG_MAGENTA        "\x1B[35m"
#define ANSI_FG_CYAN           "\x1B[36m"
#define ANSI_FG_WHITE          "\x1B[37m"
#define ANSI_FG_DEFAULT        "\x1B[39m"

/* Bright Foreground */
#define ANSI_FG_BRIGHT_BLACK   "\x1B[90m"
#define ANSI_FG_BRIGHT_RED     "\x1B[91m"
#define ANSI_FG_BRIGHT_GREEN   "\x1B[92m"
#define ANSI_FG_BRIGHT_YELLOW  "\x1B[93m"
#define ANSI_FG_BRIGHT_BLUE    "\x1B[94m"
#define ANSI_FG_BRIGHT_MAGENTA "\x1B[95m"
#define ANSI_FG_BRIGHT_CYAN    "\x1B[96m"
#define ANSI_FG_BRIGHT_WHITE   "\x1B[97m"

/* Background */
#define ANSI_BG_BLACK          "\x1B[40m"
#define ANSI_BG_RED            "\x1B[41m"
#define ANSI_BG_GREEN          "\x1B[42m"
#define ANSI_BG_YELLOW         "\x1B[43m"
#define ANSI_BG_BLUE           "\x1B[44m"
#define ANSI_BG_MAGENTA        "\x1B[45m"
#define ANSI_BG_CYAN           "\x1B[46m"
#define ANSI_BG_WHITE          "\x1B[47m"
#define ANSI_BG_DEFAULT        "\x1B[49m"

/* Bright Background */
#define ANSI_BG_BRIGHT_BLACK   "\x1B[100m"
#define ANSI_BG_BRIGHT_RED     "\x1B[101m"
#define ANSI_BG_BRIGHT_GREEN   "\x1B[102m"
#define ANSI_BG_BRIGHT_YELLOW  "\x1B[103m"
#define ANSI_BG_BRIGHT_BLUE    "\x1B[104m"
#define ANSI_BG_BRIGHT_MAGENTA "\x1B[105m"
#define ANSI_BG_BRIGHT_CYAN    "\x1B[106m"
#define ANSI_BG_BRIGHT_WHITE   "\x1B[107m"

/* Cursor */
#define ANSI_CURSOR_UP(n)      "\x1B[" #n "A"
#define ANSI_CURSOR_DOWN(n)    "\x1B[" #n "B"
#define ANSI_CURSOR_RIGHT(n)   "\x1B[" #n "C"
#define ANSI_CURSOR_LEFT(n)    "\x1B[" #n "D"
#define ANSI_CURSOR_HOME       "\x1B[H"
#define ANSI_CURSOR_POS(r,c)   "\x1B[" #r ";" #c "H"
#define ANSI_CURSOR_SAVE       "\x1B[s"
#define ANSI_CURSOR_RESTORE    "\x1B[u"
#define ANSI_CURSOR_HIDE       "\x1B[?25l"
#define ANSI_CURSOR_SHOW       "\x1B[?25h"

/* Screen */
#define ANSI_CLEAR_SCREEN      "\x1B[2J"
#define ANSI_CLEAR_LINE        "\x1B[2K"
#define ANSI_CLEAR_TO_END      "\x1B[0J"
#define ANSI_CLEAR_TO_START    "\x1B[1J"

/* Title */
#define ANSI_SET_TITLE(title)  "\x1B]0;" title "\x07"

// Functions
//=============================================================================

// Safe Casts =================================================================

internal U16 safe_cast_u16(U32 x);
internal U32 safe_cast_u32(U64 x);
internal I32 safe_cast_s32(I64 x);
internal U32 u32_from_u64_saturate(U64 x);

// Toolchain/Environment Enum Functions =======================================

internal Context_Os context_of_os(void);
internal Context_Arch context_of_arch(void);
internal Context_Compiler context_of_compiler(void);

#endif // BASE_CORE_H
