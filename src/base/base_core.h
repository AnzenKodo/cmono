#ifndef BASE_CORE_H
#define BASE_CORE_H

// External Includes
//=============================================================================

#include <stdarg.h>

// Code Keywords
//=============================================================================

#if COMPILER_CLANG || COMPILER_GCC
#   define FILE_NAME __FILE_NAME__
#else
#   define FILE_NAME __FILE__
#endif
#define LINE_NUMBER __LINE__

// Constants
//=============================================================================

global const uint32_t bitmask1  = 0x00000001;
global const uint32_t bitmask2  = 0x00000003;
global const uint32_t bitmask3  = 0x00000007;
global const uint32_t bitmask4  = 0x0000000f;
global const uint32_t bitmask5  = 0x0000001f;
global const uint32_t bitmask6  = 0x0000003f;
global const uint32_t bitmask7  = 0x0000007f;
global const uint32_t bitmask8  = 0x000000ff;
global const uint32_t bitmask9  = 0x000001ff;
global const uint32_t bitmask10 = 0x000003ff;
global const uint32_t bitmask11 = 0x000007ff;
global const uint32_t bitmask12 = 0x00000fff;
global const uint32_t bitmask13 = 0x00001fff;
global const uint32_t bitmask14 = 0x00003fff;
global const uint32_t bitmask15 = 0x00007fff;
global const uint32_t bitmask16 = 0x0000ffff;
global const uint32_t bitmask17 = 0x0001ffff;
global const uint32_t bitmask18 = 0x0003ffff;
global const uint32_t bitmask19 = 0x0007ffff;
global const uint32_t bitmask20 = 0x000fffff;
global const uint32_t bitmask21 = 0x001fffff;
global const uint32_t bitmask22 = 0x003fffff;
global const uint32_t bitmask23 = 0x007fffff;
global const uint32_t bitmask24 = 0x00ffffff;
global const uint32_t bitmask25 = 0x01ffffff;
global const uint32_t bitmask26 = 0x03ffffff;
global const uint32_t bitmask27 = 0x07ffffff;
global const uint32_t bitmask28 = 0x0fffffff;
global const uint32_t bitmask29 = 0x1fffffff;
global const uint32_t bitmask30 = 0x3fffffff;
global const uint32_t bitmask31 = 0x7fffffff;
global const uint32_t bitmask32 = 0xffffffff;
global const uint64_t bitmask33 = 0x00000001ffffffffull;
global const uint64_t bitmask34 = 0x00000003ffffffffull;
global const uint64_t bitmask35 = 0x00000007ffffffffull;
global const uint64_t bitmask36 = 0x0000000fffffffffull;
global const uint64_t bitmask37 = 0x0000001fffffffffull;
global const uint64_t bitmask38 = 0x0000003fffffffffull;
global const uint64_t bitmask39 = 0x0000007fffffffffull;
global const uint64_t bitmask40 = 0x000000ffffffffffull;
global const uint64_t bitmask41 = 0x000001ffffffffffull;
global const uint64_t bitmask42 = 0x000003ffffffffffull;
global const uint64_t bitmask43 = 0x000007ffffffffffull;
global const uint64_t bitmask44 = 0x00000fffffffffffull;
global const uint64_t bitmask45 = 0x00001fffffffffffull;
global const uint64_t bitmask46 = 0x00003fffffffffffull;
global const uint64_t bitmask47 = 0x00007fffffffffffull;
global const uint64_t bitmask48 = 0x0000ffffffffffffull;
global const uint64_t bitmask49 = 0x0001ffffffffffffull;
global const uint64_t bitmask50 = 0x0003ffffffffffffull;
global const uint64_t bitmask51 = 0x0007ffffffffffffull;
global const uint64_t bitmask52 = 0x000fffffffffffffull;
global const uint64_t bitmask53 = 0x001fffffffffffffull;
global const uint64_t bitmask54 = 0x003fffffffffffffull;
global const uint64_t bitmask55 = 0x007fffffffffffffull;
global const uint64_t bitmask56 = 0x00ffffffffffffffull;
global const uint64_t bitmask57 = 0x01ffffffffffffffull;
global const uint64_t bitmask58 = 0x03ffffffffffffffull;
global const uint64_t bitmask59 = 0x07ffffffffffffffull;
global const uint64_t bitmask60 = 0x0fffffffffffffffull;
global const uint64_t bitmask61 = 0x1fffffffffffffffull;
global const uint64_t bitmask62 = 0x3fffffffffffffffull;
global const uint64_t bitmask63 = 0x7fffffffffffffffull;
global const uint64_t bitmask64 = 0xffffffffffffffffull;

global const uint32_t bit1  = (1<<0);
global const uint32_t bit2  = (1<<1);
global const uint32_t bit3  = (1<<2);
global const uint32_t bit4  = (1<<3);
global const uint32_t bit5  = (1<<4);
global const uint32_t bit6  = (1<<5);
global const uint32_t bit7  = (1<<6);
global const uint32_t bit8  = (1<<7);
global const uint32_t bit9  = (1<<8);
global const uint32_t bit10 = (1<<9);
global const uint32_t bit11 = (1<<10);
global const uint32_t bit12 = (1<<11);
global const uint32_t bit13 = (1<<12);
global const uint32_t bit14 = (1<<13);
global const uint32_t bit15 = (1<<14);
global const uint32_t bit16 = (1<<15);
global const uint32_t bit17 = (1<<16);
global const uint32_t bit18 = (1<<17);
global const uint32_t bit19 = (1<<18);
global const uint32_t bit20 = (1<<19);
global const uint32_t bit21 = (1<<20);
global const uint32_t bit22 = (1<<21);
global const uint32_t bit23 = (1<<22);
global const uint32_t bit24 = (1<<23);
global const uint32_t bit25 = (1<<24);
global const uint32_t bit26 = (1<<25);
global const uint32_t bit27 = (1<<26);
global const uint32_t bit28 = (1<<27);
global const uint32_t bit29 = (1<<28);
global const uint32_t bit30 = (1<<29);
global const uint32_t bit31 = (1<<30);
global const uint32_t bit32 = (1<<31);
global const uint64_t bit33 = (1ull<<32);
global const uint64_t bit34 = (1ull<<33);
global const uint64_t bit35 = (1ull<<34);
global const uint64_t bit36 = (1ull<<35);
global const uint64_t bit37 = (1ull<<36);
global const uint64_t bit38 = (1ull<<37);
global const uint64_t bit39 = (1ull<<38);
global const uint64_t bit40 = (1ull<<39);
global const uint64_t bit41 = (1ull<<40);
global const uint64_t bit42 = (1ull<<41);
global const uint64_t bit43 = (1ull<<42);
global const uint64_t bit44 = (1ull<<43);
global const uint64_t bit45 = (1ull<<44);
global const uint64_t bit46 = (1ull<<45);
global const uint64_t bit47 = (1ull<<46);
global const uint64_t bit48 = (1ull<<47);
global const uint64_t bit49 = (1ull<<48);
global const uint64_t bit50 = (1ull<<49);
global const uint64_t bit51 = (1ull<<50);
global const uint64_t bit52 = (1ull<<51);
global const uint64_t bit53 = (1ull<<52);
global const uint64_t bit54 = (1ull<<53);
global const uint64_t bit55 = (1ull<<54);
global const uint64_t bit56 = (1ull<<55);
global const uint64_t bit57 = (1ull<<56);
global const uint64_t bit58 = (1ull<<57);
global const uint64_t bit59 = (1ull<<58);
global const uint64_t bit60 = (1ull<<59);
global const uint64_t bit61 = (1ull<<60);
global const uint64_t bit62 = (1ull<<61);
global const uint64_t bit63 = (1ull<<62);
global const uint64_t bit64 = (1ull<<63);

// Link Lists
//=============================================================================

// Linked List macro helpers ==================================================
#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

// Doubly-Linked-Lists ========================================================
#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
CheckNil(nil,p) ? \
((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
((p)==(l)) ? \
((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :\
(((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushFront_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
((n) == (l) ? (l) = (l)->prev : (0)),\
(CheckNil(nil,(n)->prev) ? (0) :\
((n)->prev->next = (n)->next)),\
(CheckNil(nil,(n)->next) ? (0) :\
((n)->next->prev = (n)->prev)))

// Singly-Linked, Doubly-Headed Lists (Queues) ================================
#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))
#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

// Singly-Linked, Singly-Headed Lists (Stacks) ================================
#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)

// Doubly-Linked-List helpers =================================================
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(0,f,l,n,next,prev)
#define DLLInsert(f,l,p,n) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove(f,l,n) DLLRemove_NPZ(0,f,l,n,next,prev)

// Singly-Linked, Doubly-Headed List helpers ==================================
#define SLLQueuePush_N(f,l,n,next) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront_N(f,l,n,next) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop_N(f,l,next) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop(f,l) SLLQueuePop_NZ(0,f,l,next)

// Singly-Linked, Singly-Headed List helpers ==================================
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)

// Misc. Macros
//=============================================================================

#define Unused(var)   (void)var
#define Min(A,B)     (((A)<(B))?(A):(B))
#define Max(A,B)     (((A)>(B))?(A):(B))
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))
#define IsPow2(x)       ((x != 0) && ((x & (x - 1)) == 0))
#define AlignPow2(x,b)  (((x) + (b) - 1)&(~((b) - 1)))

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
#define INVALID_CODE_PATH Assert(!"Invalid Path!")
#define NOT_IMPLEMENTED   Assert(!"Not Implemented!")

#endif // BASE_CORE_H
