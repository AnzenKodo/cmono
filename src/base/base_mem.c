internal inline bool mem_match(void const *s1, void const *s2, I64 size)
{
    bool result = false;
    U8 const *s1p8 = Cast(U8 const *)s1;
    U8 const *s2p8 = Cast(U8 const *)s2;
    if (s1 == NULL || s2 == NULL) {
        result = false;
    }
    while (size--) {
        if (*s1p8 == *s2p8) {
            result = true;
            s1p8++, s2p8++;
        } else {
            result = false;
            break;
        }
    }
    return result;
}

internal inline void *mem_copy(void *dest, void const *source, I64 n)
{
#if COMPILER_MSVC
    if (dest == NULL) {
        return NULL;
    }
    // TODO: Is this good enough?
    __movsb(Cast(U8 *)dest, Cast(U8 *)source, n);
// #elif defined(GB_SYSTEM_OSX) || defined(GB_SYSTEM_UNIX)
    // NOTE: I assume there's a reason this isn't being used elsewhere,
    //   but casting pointers as arguments to an __asm__ call is considered an
    //   error on MacOS and (I think) Linux
    // TODO: Figure out how to refactor the asm code so it works on MacOS,
    //   since this is probably not the way the author intended this to work.
    // memcpy(dest, source, n);
#elif ARCH_X86
    if (dest == NULL) {
        return NULL;
    }
    void *dest_copy = dest;
    __asm__ __volatile__("rep movsb" : "+D"(dest_copy), "+S"(source), "+c"(n) : : "memory");
#else
    U8 *d = Cast(U8 *)dest;
    U8 const *s = Cast(U8 const *)source;
    U32 w, x;
    if (dest == NULL) {
        return NULL;
    }
    for (; IntFromPtr(s) % 4 && n; n--) {
        *d++ = *s++;
    }
    if (IntFromPtr(d) % 4 == 0) {
        for (; n >= 16;
             s += 16, d += 16, n -= 16) {
            *Cast(U32 *)(d+ 0) = *Cast(U32 *)(s+ 0);
            *Cast(U32 *)(d+ 4) = *Cast(U32 *)(s+ 4);
            *Cast(U32 *)(d+ 8) = *Cast(U32 *)(s+ 8);
            *Cast(U32 *)(d+12) = *Cast(U32 *)(s+12);
        }
        if (n & 8) {
            *Cast(U32 *)(d+0) = *Cast(U32 *)(s+0);
            *Cast(U32 *)(d+4) = *Cast(U32 *)(s+4);
            d += 8;
            s += 8;
        }
        if (n&4) {
            *Cast(U32 *)(d+0) = *Cast(U32 *)(s+0);
            d += 4;
            s += 4;
        }
        if (n&2) {
            *d++ = *s++; *d++ = *s++;
        }
        if (n&1) {
            *d = *s;
        }
        return dest;
    }
    if (n >= 32) {
    #if __BYTE_ORDER == __BIG_ENDIAN
    #define LS <<
    #define RS >>
    #else
    #define LS >>
    #define RS <<
    #endif
        switch (IntFromPtr(d) % 4) {
        case 1: {
            w = *Cast(U32 *)s;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            n -= 3;
            while (n > 16) {
                x = *Cast(U32 *)(s+1);
                *Cast(U32 *)(d+0)  = (w LS 24) | (x RS 8);
                w = *Cast(U32 *)(s+5);
                *Cast(U32 *)(d+4)  = (x LS 24) | (w RS 8);
                x = *Cast(U32 *)(s+9);
                *Cast(U32 *)(d+8)  = (w LS 24) | (x RS 8);
                w = *Cast(U32 *)(s+13);
                *Cast(U32 *)(d+12) = (x LS 24) | (w RS 8);
                s += 16;
                d += 16;
                n -= 16;
            }
        } break;
        case 2: {
            w = *Cast(U32 *)s;
            *d++ = *s++;
            *d++ = *s++;
            n -= 2;
            while (n > 17) {
                x = *Cast(U32 *)(s+2);
                *Cast(U32 *)(d+0)  = (w LS 16) | (x RS 16);
                w = *Cast(U32 *)(s+6);
                *Cast(U32 *)(d+4)  = (x LS 16) | (w RS 16);
                x = *Cast(U32 *)(s+10);
                *Cast(U32 *)(d+8)  = (w LS 16) | (x RS 16);
                w = *Cast(U32 *)(s+14);
                *Cast(U32 *)(d+12) = (x LS 16) | (w RS 16);
                s += 16;
                d += 16;
                n -= 16;
            }
        } break;
        case 3: {
            w = *Cast(U32 *)s;
            *d++ = *s++;
            n -= 1;
            while (n > 18) {
                x = *Cast(U32 *)(s+3);
                *Cast(U32 *)(d+0)  = (w LS 8) | (x RS 24);
                w = *Cast(U32 *)(s+7);
                *Cast(U32 *)(d+4)  = (x LS 8) | (w RS 24);
                x = *Cast(U32 *)(s+11);
                *Cast(U32 *)(d+8)  = (w LS 8) | (x RS 24);
                w = *Cast(U32 *)(s+15);
                *Cast(U32 *)(d+12) = (x LS 8) | (w RS 24);
                s += 16;
                d += 16;
                n -= 16;
            }
        } break;
        default: break; // NOTE: Do nowt!
        }
    #undef LS
    #undef RS
        if (n & 16) {
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        }
        if (n & 8) {
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        }
        if (n & 4) {
            *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        }
        if (n & 2) {
            *d++ = *s++; *d++ = *s++;
        }
        if (n & 1) {
            *d = *s;
        }
    }
#endif
    return dest;
}

internal inline void *mem_move(void *dest, void const *source, I64 n)
{
    U8 *d = Cast(U8 *)dest;
    U8 const *s = Cast(U8 const *)source;
    if (dest == NULL) {
        return NULL;
    }
    if (d == s) {
        return d;
    }
    if (s+n <= d || d+n <= s) { // NOTE: Non-overlapping
        return mem_copy(d, s, n);
    }
    if (d < s) {
        if (IntFromPtr(s) % sizeof(I64) == IntFromPtr(d) % sizeof(I64)) {
            while (IntFromPtr(d) % sizeof(I64)) {
                if (!n--) return dest;
                *d++ = *s++;
            }
            while (n >= (I64)sizeof(I64)) {
                *Cast(I64 *)d = *Cast(I64 *)s;
                n -= sizeof(I64);
                d += sizeof(I64);
                s += sizeof(I64);
            }
        }
        for (; n; n--) *d++ = *s++;
    } else {
        if ((IntFromPtr(s) % sizeof(I64)) == (IntFromPtr(d) % sizeof(I64))) {
            while (IntFromPtr(d+n) % sizeof(I64)) {
                if (!n--)
                    return dest;
                d[n] = s[n];
            }
            while (n >= (I64)sizeof(I64)) {
                n -= sizeof(I64);
                *Cast(I64 *)(d+n) = *Cast(I64 *)(s+n);
            }
        }
        while (n) n--, d[n] = s[n];
    }
    return dest;
}

internal inline void *mem_set(void *dest, U8 c, I64 n)
{
    U8 *s = Cast(U8 *)dest;
    I64 k;
    U32 c32 = ((U32)-1)/255 * c;
    if (dest == NULL) {
        return NULL;
    }
    if (n == 0) { return dest; }
    s[0] = s[n-1] = c;
    if (n < 3) { return dest; }
    s[1] = s[n-2] = c;
    s[2] = s[n-3] = c;
    if (n < 7) { return dest; }
    s[3] = s[n-4] = c;
    if (n < 9) { return dest; }
    k = -Cast(I64)s & 3;
    s += k;
    n -= k;
    n &= -4;
    *Cast(U32 *)(s+0) = c32;
    *Cast(U32 *)(s+n-4) = c32;
    if (n < 9) { return dest; }
    *Cast(U32 *)(s +  4)    = c32;
    *Cast(U32 *)(s +  8)    = c32;
    *Cast(U32 *)(s+n-12) = c32;
    *Cast(U32 *)(s+n- 8) = c32;
    if (n < 25) { return dest; }
    *Cast(U32 *)(s + 12) = c32;
    *Cast(U32 *)(s + 16) = c32;
    *Cast(U32 *)(s + 20) = c32;
    *Cast(U32 *)(s + 24) = c32;
    *Cast(U32 *)(s+n-28) = c32;
    *Cast(U32 *)(s+n-24) = c32;
    *Cast(U32 *)(s+n-20) = c32;
    *Cast(U32 *)(s+n-16) = c32;
    k = 24 + (IntFromPtr(s) & 4);
    s += k;
    n -= k;
    {
        U64 c64 = (Cast(U64)c32 << 32) | c32;
        while (n > 31) {
            *Cast(U64 *)(s+0) = c64;
            *Cast(U64 *)(s+8) = c64;
            *Cast(U64 *)(s+16) = c64;
            *Cast(U64 *)(s+24) = c64;
            n -= 32;
            s += 32;
        }
    }
    return dest;
}

internal I32 mem_is_zero(void *ptr, U64 size){
  I32 result = 1;

  // break down size
  U64 extra = (size&0x7);
  U64 count8 = (size >> 3);

  // check with 8-byte stride
  U64 *p64 = (U64*)ptr;
  if(result)
  {
    for (U64 i = 0; i < count8; i += 1, p64 += 1){
      if (*p64 != 0){
        result = 0;
        goto done;
      }
    }
  }

  // check extra
  if(result)
  {
    U8 *p8 = (U8*)p64;
    for (U64 i = 0; i < extra; i += 1, p8 += 1){
      if (*p8 != 0){
        result = 0;
        goto done;
      }
    }
  }

  done:;
  return(result);
}

