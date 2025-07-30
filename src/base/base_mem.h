#ifndef BASE_MEM_H
#define BASE_MEM_H

#define MemoryCopyStruct(d,s)  mem_copy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   mem_copy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) mem_copy((d),(s),sizeof(*(d))*(c))
#define MemoryCopyStr8(dst, s) mem_copy(dst, (s).str, (s).size)

internal inline bool mem_match(void const *s1, void const *s2, I64 size);
internal inline void *mem_copy(void *dest, void const *source, I64 n);
internal inline void *mem_move(void *dest, void const *source, I64 n);
internal inline void *mem_set(void *dest, U8 c, I64 n);
internal I32 mem_is_zero(void *ptr, U64 size);

#endif // BASE_MEM_H
