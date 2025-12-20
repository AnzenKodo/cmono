#ifndef BASE_MATH_H
#define BASE_MATH_H

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

#define MATH_PI           3.14159265358979323846264338327950288f

// Vector Ops
//=============================================================================

// 2-Vectors ==================================================================

internal Vec2_I16 vec2_i16(int16_t x, int16_t y);
internal Vec2_I32 vec2_i32(int32_t x, int32_t y);
internal Vec2_I64 vec2_i64(int64_t x, int64_t y);
internal Vec2_F32 vec2_f32(float x, float y);

// 3-Vectors ==================================================================

internal Vec3_I32 vec3_i32(int32_t x, int32_t y, int32_t z);
internal Vec3_F32 vec3_f32(float x, float y, float z);

// 4-vectors ==================================================================

internal Vec4_I32 vec4_i32(int32_t x, int32_t y, int32_t z, int32_t w);
internal Vec4_F32 vec4_f32(float x, float y, float z, float w);

// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal Rng1_I32 rng1_i32(int32_t min, int32_t max);
internal Rng1_I64 rng1_i64(int64_t min, int64_t max);
internal Rng1_U32 rng1_u32(uint32_t min, uint32_t max);
internal Rng1_U64 rng1_u64(uint64_t min, uint64_t max);
internal Rng1_F32 rng1_f32(float min, float max);

internal int32_t  dim1_i32(Rng1_I32 r);
internal int64_t  dim1_i64(Rng1_I64 r);
internal uint32_t dim1_u32(Rng1_U32 r);
internal uint64_t dim1_u64(Rng1_U64 r);
internal float    dim1_f32(Rng1_F32 r);

// 2 Range (Rectangles) =======================================================

internal Rng2_I16 rng2_i16(Vec2_I16 min, Vec2_I16 max);
internal Rng2_I32 rng2_i32(Vec2_I32 min, Vec2_I32 max);
internal Rng2_I64 rng2_i64(Vec2_I64 min, Vec2_I64 max);
internal Rng2_F32 rng2_f32(Vec2_F32 min, Vec2_F32 max);

#define rng2_i16_p(x, y, z, w) rng2_i16(vec2_i16((x), (y)), vec2_i16((z), (w)))
#define rng2_i32_p(x, y, z, w) rng2_i32(vec2_i32((x), (y)), vec2_i32((z), (w)))
#define rng2_i64_p(x, y, z, w) rng2_i64(vec2_i64((x), (y)), vec2_i64((z), (w)))
#define rng2_f32_p(x, y, z, w) rng2_f32(vec2_f32((x), (y)), vec2_f32((z), (w)))

internal Vec2_I16 dim2_i16(Rng2_I16 r);
internal Vec2_I32 dim2_i32(Rng2_I32 r);
internal Vec2_I64 dim2_i64(Rng2_I64 r);
internal Vec2_F32 dim2_f32(Rng2_F32 r);

// Random Number
//=============================================================================

internal uint32_t math_random_u32(uint32_t seed);

#endif // BASE_MATH_H
