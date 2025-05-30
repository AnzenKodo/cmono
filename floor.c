// floor function for floats with SSE1 or SSE2

#include <emmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#define __rdtsc() __builtin_ia32_rdtsc()
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>

inline static float std_floor(float x)
{
    // when compiled with -msse4.1 or with /arch:AVX this
    // will actually produce "roundss" instruction
    return floorf(x);
}

// #if defined(__clang__) || defined(__GNUC__)
// __attribute__((target("sse4.1")))
// #endif
// full float range, with INFs & NANs
// inline static float sse4_floor(float x)
// {
//     __m128 f = _mm_set_ss(x);
//     __m128 r = _mm_floor_ss(f, f);
//     return _mm_cvtss_f32(r);
// }

// #if defined(__clang__) || defined(__GNUC__)
// __attribute__((target("avx2")))
// #endif
// full float range, with INFs & NANs
// inline static float avx2_floor(float x)
// {
//     __m256 f = _mm256_set1_ps(x);
//     __m256 r = _mm256_floor_ps(f);
//     return _mm256_cvtss_f32(r);
// }

#define CHECK(f, x, expected, func) do \
{                                                                                                  \
    float actual = func(f);                                                                        \
    if (fpclassify(actual) == FP_NAN && fpclassify(expected) == FP_NAN) {} /* OK, both are NaNs */ \
    else if (actual != expected)                                                                   \
    {                                                                                              \
        printf("ERROR: f=%1.8e (0x%08x) floorf=%1.8e %s=%1.8e\n", f, x, expected, #func, actual);  \
        return 0;                                                                                  \
    }                                                                                              \
} while (0)

#if defined(_MSC_VER) && !defined(__clang__)
#define DO_NOT_OPTIMIZE(x) volatile float temp = x
#else
#define DO_NOT_OPTIMIZE(x) __asm__ __volatile__("" : "+x"(x) : : "memory")
#endif

#define BENCH_ITERS 99999
#define BENCH_COUNT 9999

#define BENCH(func) do                                         \
{                                                              \
    unsigned long long best = ~0ULL;                           \
    for (int k=0; k<BENCH_ITERS; k++)                          \
    {                                                          \
        volatile float f = 0;                                  \
        unsigned long long t1 = __rdtsc();                     \
        for (int i=0; i<BENCH_COUNT; i++)                      \
        {                                                      \
            float r = func(f);                                 \
            DO_NOT_OPTIMIZE(r);                                \
        }                                                      \
        unsigned long long t2 = __rdtsc();                     \
        if (t2 - t1 < best) best = t2 - t1;                    \
    }                                                          \
    printf("%-23s%5.2f\n", #func, (double)best / BENCH_COUNT); \
} while (0)

int main()
{
    volatile unsigned int x = 0;
    do
    {
        if ((x % (1<<26) == 0))
        {
            printf(".");
            fflush(stdout);
        }

        float f;
        memcpy((void*)&f, (void*)&x, 4);

        float expected = std_floor(f);

        // CHECK(f, x, expected, avx2_floor);
        // CHECK(f, x, expected, sse4_floor);

        x++;
    }
    while (x != 0);

    printf(" OK!\n");

    BENCH(std_floor);
    // BENCH(avx2_floor);
    // BENCH(sse4_floor);
}
