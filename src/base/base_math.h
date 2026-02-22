#ifndef BASE_MATH_H
#define BASE_MATH_H

// NOTE(ak): the code reference taken from:
// - https://github.com/gingerBill/gb/blob/master/gb_math.h
// - https://github.com/HandmadeMath/HandmadeMath
// - https://github.com/EpicGamesExt/raddebugger

// Base
//=============================================================================

typedef enum Base {
    Base_2  = 2,
    Base_8  = 8,
    Base_10 = 10,
    Base_16 = 16,
} Base;

// Vector Types
//=============================================================================

// 2-Vectors ==================================================================

typedef union Vec2_F32 Vec2_F32;
union Vec2_F32
{
    struct
    {
        float x;
        float y;
    };
    float v[2];
};

typedef union Vec2_I64 Vec2_I64;
union Vec2_I64
{
    struct
    {
        int64_t x;
        int64_t y;
    };
    int64_t v[2];
};

typedef union Vec2_I32 Vec2_I32;
union Vec2_I32
{
    struct
    {
        int32_t x;
        int32_t y;
    };
    int32_t v[2];
};

typedef union Vec2_I16 Vec2_I16;
union Vec2_I16
{
    struct
    {
        int16_t x;
        int16_t y;
    };
    int16_t v[2];
};

typedef union Vec2_U64 Vec2_U64;
union Vec2_U64
{
    struct
    {
        uint64_t x;
        uint64_t y;
    };
    uint64_t v[2];
};

typedef union Vec2_U32 Vec2_U32;
union Vec2_U32
{
    struct
    {
        uint32_t x;
        uint32_t y;
    };
    uint32_t v[2];
};

typedef union Vec2_U16 Vec2_U16;
union Vec2_U16
{
    struct
    {
        uint16_t x;
        uint16_t y;
    };
    uint16_t v[2];
};

// 3-Vectors ==================================================================

typedef union Vec3_F32 Vec3_F32;
union Vec3_F32
{
    struct
    {
        float x;
        float y;
        float z;
    };
    struct
    {
        Vec2_F32 xy;
        float _z0;
    };
    struct
    {
        float _x0;
        Vec2_F32 yz;
    };
    float v[3];
};

typedef union Vec3_I32 Vec3_I32;
union Vec3_I32
{
    struct
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };
    struct
    {
        Vec2_I32 xy;
        int32_t _z0;
    };
    struct
    {
        int32_t _x0;
        Vec2_I32 yz;
    };
    int32_t v[3];
};

// 4-vectors ==================================================================

typedef union Vec4_F32 Vec4_F32;
union Vec4_F32
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct
    {
        Vec2_F32 xy;
        Vec2_F32 zw;
    };
    struct
    {
        Vec3_F32 xyz;
        float _z0;
    };
    struct
    {
        float _x0;
        Vec3_F32 yzw;
    };
    float v[4];
};

typedef union Vec4_I32 Vec4_I32;
union Vec4_I32
{
    struct
    {
        int32_t x;
        int32_t y;
        int32_t z;
        int32_t w;
    };
    struct
    {
        Vec2_I32 xy;
        Vec2_I32 zw;
    };
    struct
    {
        Vec3_I32 xyz;
        int32_t _z0;
    };
    struct
    {
        int32_t _x0;
        Vec3_I32 yzw;
    };
    int32_t v[4];
};

// Range Types
//=============================================================================

// 1 Range ====================================================================

typedef union Rng1_U32 Rng1_U32;
union Rng1_U32
{
    struct
    {
        uint32_t min;
        uint32_t max;
    };
    uint32_t v[2];
};

typedef union Rng1_I32 Rng1_I32;
union Rng1_I32
{
    struct
    {
        int32_t min;
        int32_t max;
    };
    int32_t v[2];
};

typedef union Rng1_U64 Rng1_U64;
union Rng1_U64
{
    struct
    {
        uint64_t min;
        uint64_t max;
    };
    uint64_t v[2];
};

typedef union Rng1_I64 Rng1_I64;
union Rng1_I64
{
    struct
    {
        int64_t min;
        int64_t max;
    };
    int64_t v[2];
};

typedef union Rng1_F32 Rng1_F32;
union Rng1_F32
{
    struct
    {
        float min;
        float max;
    };
    float v[2];
};

// 2 Range (Rectangles) =======================================================

typedef union Rng2_I16 Rng2_I16;
union Rng2_I16
{
    struct
    {
        Vec2_I16 min;
        Vec2_I16 max;
      };
    struct
    {
        Vec2_I16 p0;
        Vec2_I16 p1;
    };
    struct
    {
        int16_t x0;
        int16_t y0;
        int16_t x1;
        int16_t y1;
    };
    Vec2_I16 v[2];
};

typedef union Rng2_I32 Rng2_I32;
union Rng2_I32
{
    struct
    {
        Vec2_I32 min;
        Vec2_I32 max;
    };
    struct
    {
        Vec2_I32 p0;
        Vec2_I32 p1;
    };
    struct
    {
        int32_t x0;
        int32_t y0;
        int32_t x1;
        int32_t y1;
    };
    Vec2_I32 v[2];
};

typedef union Rng2_F32 Rng2_F32;
union Rng2_F32
{
    struct
    {
        Vec2_F32 min;
        Vec2_F32 max;
    };
    struct
    {
        Vec2_F32 p0;
        Vec2_F32 p1;
    };
    struct
    {
        float x0;
        float y0;
        float x1;
        float y1;
    };
    Vec2_F32 v[2];
};

typedef union Rng2_I64 Rng2_I64;
union Rng2_I64
{
    struct
    {
        Vec2_I64 min;
        Vec2_I64 max;
    };
    struct
    {
        Vec2_I64 p0;
        Vec2_I64 p1;
    };
    struct
    {
        int64_t x0;
        int64_t y0;
        int64_t x1;
        int64_t y1;
    };
    Vec2_I64 v[2];
};

// Units
//=============================================================================

// Numerical Units
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

// Unit of Information
#define KB(n)  (((uint64_t)(n)) << 10)
#define MB(n)  (((uint64_t)(n)) << 20)
#define GB(n)  (((uint64_t)(n)) << 30)
#define TB(n)  (((uint64_t)(n)) << 40)

// Constants
//=============================================================================

#define EPSILON_F32      1.19209290e-7f
#define ZERO_F32         0.0f
#define ONE_F32          1.0f
#define TWO_THIRDS_F32   0.666666666666666666666666666666666666667f
#define TAU_F32          6.28318530717958647692528676655900576f
#define PI_F32           3.14159265358979323846264338327950288f
#define ONE_OVER_TAU_F32 0.159154943091895335768883763372514362f
#define ONE_OVER_PI_F32  0.318309886183790671537767526745028724f
#define TAU_OVER_2_F32   3.14159265358979323846264338327950288f
#define TAU_OVER_4_F32   1.570796326794896619231321691639751442f
#define TAU_OVER_8_F32   0.785398163397448309615660845819875721f
#define E_F32            2.7182818284590452353602874713526625f
#define SQRT_TWO_F32     1.41421356237309504880168872420969808f
#define SQRT_THREE_F32   1.73205080756887729352744634150587236f
#define SQRT_FIVE_F32    2.23606797749978969640917366873127623f
#define LOG_TWO_F32      0.693147180559945309417232121458176568f
#define LOG_TEN_F32      2.30258509299404568401799145468436421f

#define EPSILON_F64      1.19209290e-7
#define ZERO_F64         0.0
#define ONE_F64          1.0
#define TWO_THIRDS_F64   0.666666666666666666666666666666666666667
#define TAU_F64          6.28318530717958647692528676655900576
#define PI_F64           3.14159265358979323846264338327950288
#define ONE_OVER_TAU_F64 0.159154943091895335768883763372514362
#define ONE_OVER_PI_F64  0.318309886183790671537767526745028724
#define TAU_OVER_2_F64   3.14159265358979323846264338327950288
#define TAU_OVER_4_F64   1.570796326794896619231321691639751442
#define TAU_OVER_8_F64   0.785398163397448309615660845819875721
#define E_F64            2.7182818284590452353602874713526625
#define SQRT_TWO_F64     1.41421356237309504880168872420969808
#define SQRT_THREE_F64   1.73205080756887729352744634150587236
#define SQRT_FIVE_F64    2.23606797749978969640917366873127623
#define LOG_TWO_F64      0.693147180559945309417232121458176568
#define LOG_TEN_F64      2.30258509299404568401799145468436421

// Scalar Math Ops
//=============================================================================

#define IsPow2(x)      ((x != 0) && ((x & (x - 1)) == 0))
#define Min(A,B)       (((A)<(B))?(A):(B))
#define Max(A,B)       (((A)>(B))?(A):(B))
#define Min3(a, b, c)  Min(Min(a, b), c)
#define Max3(a, b, c)  Max(Max(a, b), c)
#define Clamp(A,X,B)   (((X)<(A))?(A):((X)>(B))?(B):(X))
#define Square(x)      ((x)*(x))
#define Cube(x)        ((x)*(x)*(x))
#define Abs(x)         ((x) > 0 ? (x) : -(x))
#define Sign(x)        ((x) >= 0 ? 1 : -1)
#define Mod(a, m)      (((a) % (m)) >= 0 ? ((a) % (m)) : (((a) % (m)) + (m)))

// NOTE(aman.v): function list comes: https://en.wikipedia.org/wiki/C_mathematical_functions

internal float remainder_f32(float x, float y);
internal float fmod_f32(float x, float y);

internal double remainder_f64(double x, double y);
internal double fmod_f64(double x, double y);

// TODO(ak): more scalar math ops functions
// - div       computes the quotient and remainder of integer division
// - remquo    signed remainder as well as the three last bits of the division operation
// - fma       fused multiply-add operation
// - fdim      positive difference of two floating-point values

// Exponential functions ======================================================

internal float exp_f32(float a);
internal float exp2_f32(float x);
internal float log_f32(float a);
internal float log2_f32(float x);

internal double exp_f64(double a);
internal double exp2_f64(double x);
internal double log_f64(double a);
internal double log2_f64(double x);

// TODO(ak): more exponential functions
// - expm1	returns e raised to the given power, minus one
// - log10	computes common logarithm (to base 10)
// - log1p	computes natural logarithm (to base e) of 1 plus the given number
// - ilogb	extracts exponent of the number
// - logb	extracts exponent of the number

// Power functions ============================================================

internal float sqrt_f32(float number);
internal float rsqrt_f32(float a);
internal float pow_f32(float a, float b);

internal double sqrt_f64(double number);
internal double rsqrt_f64(double a);
internal double pow_f64(double a, double b);

// TODO(ak): more power functions
// - cbrt
// - hypot

// Trigonometric functions ====================================================

internal float sin_f32(float a);
internal float cos_f32(float a);
internal float tan_f32(float radians);
internal float asin_f32(float a);
internal float acos_f32(float a);
internal float atan_f32(float a);
internal float atan2_f32(float y, float x);

internal double sin_f64(double a);
internal double cos_f64(double a);
internal double tan_f64(double radians);
internal double asin_f64(double a);
internal double acos_f64(double a);
internal double atan_f64(double a);
internal double atan2_f64(double y, double x);

// Hyperbolic functions =======================================================

// TODO(ak): more hyperbolic functions
// - sinh	computes hyperbolic sine
// - cosh	computes hyperbolic cosine
// - tanh	computes hyperbolic tangent
// - asinh	computes hyperbolic arc sine
// - acosh	computes hyperbolic arc cosine
// - atanh	computes hyperbolic arc tangent

// Gamma functions ============================================================

// TODO(ak): more gamma functions
// - lgamma	computes natural logarithm of the absolute value of the gamma function
// - tgamma	computes gamma function

// Nearest integer floating-point operations ==================================

internal float round_f32(float x);
internal float floor_f32(float x);
internal float ceil_f32(float x);

internal double round_f64(double x);
internal double floor_f64(double x);
internal double ceil_f64(double x);

// TODO(ak): more nearest integer floating-point operations functions
// - trunc     returns the nearest integer not greater in magnitude than the given value
// - nearbyint returns the nearest integer using current rounding mode
// - rint      returns the nearest integer using current rounding mode with exception if the result differs

// Floating-point manipulation functions ======================================

internal float copysign_f32(float x, float y);

internal double copysign_f64(double x, double y);

// TODO(ak): more floating-point manipulation functions functions
// - frexp	decomposes a number into significand and a power of 2
// - ldexp	multiplies a number by 2 raised to a power
// - modf	decomposes a number into integer and fractional parts
// - scalbn
// - scalbln	multiplies a number by FLT_RADIX raised to a power
// - nextafter
// - nexttoward	returns next representable floating-point value towards the given value

// Random-number generation
//=============================================================================

internal uint32_t rand_u32(uint32_t seed);

// Vector Ops
//=============================================================================



// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal int32_t  rng1_dim_i32(Rng1_I32 r);
internal int64_t  rng1_dim_i64(Rng1_I64 r);
internal uint32_t rng1_dim_u32(Rng1_U32 r);
internal uint64_t rng1_dim_u64(Rng1_U64 r);
internal float    rng1_dim_f32(Rng1_F32 r);

// 2 Range (Rectangles) =======================================================

internal Vec2_I16 rng2_dim_i16(Rng2_I16 r);
internal Vec2_I32 rng2_dim_i32(Rng2_I32 r);
internal Vec2_I64 rng2_dim_i64(Rng2_I64 r);
internal Vec2_F32 rng2_dim_f32(Rng2_F32 r);

#endif // BASE_H
