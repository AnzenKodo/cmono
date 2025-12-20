internal inline bool mem_match(void const *s1, void const *s2, int64_t size)
{
    bool result = false;
    uint8_t const *s1p8 = (uint8_t const *)s1;
    uint8_t const *s2p8 = (uint8_t const *)s2;
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

internal inline void *mem_copy(void *dest, void const *source, int64_t n)
{
#if COMPILER_MSVC
    if (dest == NULL) {
        return NULL;
    }
    // TODO: Is this good enough?
    __movsb((uint8_t *)dest, (uint8_t *)source, n);
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
    uint8_t *d = (uint8_t *)dest;
    uint8_t const *s = (uint8_t const *)source;
    uint32_t w, x;
    if (dest == NULL) {
        return NULL;
    }
    for (; IntFromPtr(s) % 4 && n; n--) {
        *d++ = *s++;
    }
    if (IntFromPtr(d) % 4 == 0) {
        for (; n >= 16;
             s += 16, d += 16, n -= 16) {
            *(uint32_t *)(d+ 0) = *(uint32_t *)(s+ 0);
            *(uint32_t *)(d+ 4) = *(uint32_t *)(s+ 4);
            *(uint32_t *)(d+ 8) = *(uint32_t *)(s+ 8);
            *(uint32_t *)(d+12) = *(uint32_t *)(s+12);
        }
        if (n & 8) {
            *(uint32_t *)(d+0) = *(uint32_t *)(s+0);
            *(uint32_t *)(d+4) = *(uint32_t *)(s+4);
            d += 8;
            s += 8;
        }
        if (n&4) {
            *(uint32_t *)(d+0) = *(uint32_t *)(s+0);
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
            w = *(uint32_t *)s;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            n -= 3;
            while (n > 16) {
                x = *(uint32_t *)(s+1);
                *(uint32_t *)(d+0)  = (w LS 24) | (x RS 8);
                w = *(uint32_t *)(s+5);
                *(uint32_t *)(d+4)  = (x LS 24) | (w RS 8);
                x = *(uint32_t *)(s+9);
                *(uint32_t *)(d+8)  = (w LS 24) | (x RS 8);
                w = *(uint32_t *)(s+13);
                *(uint32_t *)(d+12) = (x LS 24) | (w RS 8);
                s += 16;
                d += 16;
                n -= 16;
            }
        } break;
        case 2: {
            w = *(uint32_t *)s;
            *d++ = *s++;
            *d++ = *s++;
            n -= 2;
            while (n > 17) {
                x = *(uint32_t *)(s+2);
                *(uint32_t *)(d+0)  = (w LS 16) | (x RS 16);
                w = *(uint32_t *)(s+6);
                *(uint32_t *)(d+4)  = (x LS 16) | (w RS 16);
                x = *(uint32_t *)(s+10);
                *(uint32_t *)(d+8)  = (w LS 16) | (x RS 16);
                w = *(uint32_t *)(s+14);
                *(uint32_t *)(d+12) = (x LS 16) | (w RS 16);
                s += 16;
                d += 16;
                n -= 16;
            }
        } break;
        case 3: {
            w = *(uint32_t *)s;
            *d++ = *s++;
            n -= 1;
            while (n > 18) {
                x = *(uint32_t *)(s+3);
                *(uint32_t *)(d+0)  = (w LS 8) | (x RS 24);
                w = *(uint32_t *)(s+7);
                *(uint32_t *)(d+4)  = (x LS 8) | (w RS 24);
                x = *(uint32_t *)(s+11);
                *(uint32_t *)(d+8)  = (w LS 8) | (x RS 24);
                w = *(uint32_t *)(s+15);
                *(uint32_t *)(d+12) = (x LS 8) | (w RS 24);
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

internal inline void *mem_move(void *dest, void const *source, int64_t n)
{
    uint8_t *d = (uint8_t *)dest;
    uint8_t const *s = (uint8_t const *)source;
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
        if (IntFromPtr(s) % sizeof(int64_t) == IntFromPtr(d) % sizeof(int64_t)) {
            while (IntFromPtr(d) % sizeof(int64_t)) {
                if (!n--) return dest;
                *d++ = *s++;
            }
            while (n >= (int64_t)sizeof(int64_t)) {
                *(int64_t *)d = *(int64_t *)s;
                n -= sizeof(int64_t);
                d += sizeof(int64_t);
                s += sizeof(int64_t);
            }
        }
        for (; n; n--) *d++ = *s++;
    } else {
        if ((IntFromPtr(s) % sizeof(int64_t)) == (IntFromPtr(d) % sizeof(int64_t))) {
            while (IntFromPtr(d+n) % sizeof(int64_t)) {
                if (!n--)
                    return dest;
                d[n] = s[n];
            }
            while (n >= (int64_t)sizeof(int64_t)) {
                n -= sizeof(int64_t);
                *(int64_t *)(d+n) = *(int64_t *)(s+n);
            }
        }
        while (n) n--, d[n] = s[n];
    }
    return dest;
}

internal inline void *mem_set(void *dest, uint8_t c, int64_t n)
{
    uint8_t *s = (uint8_t *)dest;
    int64_t k;
    uint32_t c32 = ((uint32_t)-1)/255 * c;
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
    k = -(int64_t)s & 3;
    s += k;
    n -= k;
    n &= -4;
    *(uint32_t *)(s+0) = c32;
    *(uint32_t *)(s+n-4) = c32;
    if (n < 9) { return dest; }
    *(uint32_t *)(s +  4)    = c32;
    *(uint32_t *)(s +  8)    = c32;
    *(uint32_t *)(s+n-12) = c32;
    *(uint32_t *)(s+n- 8) = c32;
    if (n < 25) { return dest; }
    *(uint32_t *)(s + 12) = c32;
    *(uint32_t *)(s + 16) = c32;
    *(uint32_t *)(s + 20) = c32;
    *(uint32_t *)(s + 24) = c32;
    *(uint32_t *)(s+n-28) = c32;
    *(uint32_t *)(s+n-24) = c32;
    *(uint32_t *)(s+n-20) = c32;
    *(uint32_t *)(s+n-16) = c32;
    k = 24 + (IntFromPtr(s) & 4);
    s += k;
    n -= k;
    {
        uint64_t c64 = ((uint64_t)c32 << 32) | c32;
        while (n > 31) {
            *(uint64_t *)(s+0) = c64;
            *(uint64_t *)(s+8) = c64;
            *(uint64_t *)(s+16) = c64;
            *(uint64_t *)(s+24) = c64;
            n -= 32;
            s += 32;
        }
    }
    return dest;
}

internal int32_t mem_is_zero(void *ptr, uint64_t size){
  int32_t result = 1;

  // break down size
  uint64_t extra = (size&0x7);
  uint64_t count8 = (size >> 3);

  // check with 8-byte stride
  uint64_t *p64 = (uint64_t*)ptr;
  if(result)
  {
    for (uint64_t i = 0; i < count8; i += 1, p64 += 1){
      if (*p64 != 0){
        result = 0;
        goto done;
      }
    }
  }

  // check extra
  if(result)
  {
    uint8_t *p8 = (uint8_t*)p64;
    for (uint64_t i = 0; i < extra; i += 1, p8 += 1){
      if (*p8 != 0){
        result = 0;
        goto done;
      }
    }
  }

  done:;
  return(result);
}

