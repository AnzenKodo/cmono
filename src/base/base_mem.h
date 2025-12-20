#ifndef BASE_MEM_H
#define BASE_MEM_H

internal inline void *mem_copy(void *dest, void const *source, int64_t n);
#define MemoryCopyStruct(d,s)  mem_copy((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   mem_copy((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) mem_copy((d),(s),sizeof(*(d))*(c))
#define MemoryCopyStr8(dst, s) mem_copy(dst, (s).str, (s).size)

internal inline void *mem_set(void *dest, uint8_t c, int64_t n);
#define MemorySetZero(s,z)       mem_set((s),0,(z))
#define MemorySetZeroStruct(s)   MemorySetZero((s),sizeof(*(s)))
#define MemorySetZeroArray(a)    MemorySetZero((a),sizeof(a))
#define MemorySetZeroTyped(m,c)  MemorySetZero((m),sizeof(*(m))*(c))

internal inline bool mem_match(void const *s1, void const *s2, int64_t size);
internal inline void *mem_move(void *dest, void const *source, int64_t n);
internal int32_t mem_is_zero(void *ptr, uint64_t size);

#endif // BASE_MEM_H
