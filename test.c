#include <stdio.h>

typedef unsigned int U32;

// Your random function
static inline U32 math_random_u32(U32 seed) {
    seed = (seed << 13) ^ seed;
    return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
}

int main() {
    U32 seed = 12345; // You can use any seed value
    int N = 10;       // Number of random bits you want

    for (int i = 0; i < N; ++i) {
        seed = math_random_u32(seed + seed); // Update seed each time
        int random_bit = seed & 1;    // Extract least significant bit (0 or 1)
        printf("%d ", random_bit);
    }
    printf("\n");
    return 0;
}
