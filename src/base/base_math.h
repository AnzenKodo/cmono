#ifndef BASE_MATH_H
#define BASE_MATH_H

// Vector Types
//====================================================================

// 2-Vectors ==================================================================

typedef union Vec2F32 Vec2F32;
union Vec2F32
{
    struct
    {
        F32 x;
        F32 y;
    };
    F32 v[2];
};

typedef union Vec2I64 Vec2I64;
union Vec2I64
{
    struct
    {
        I64 x;
        I64 y;
    };
    I64 v[2];
};

typedef union Vec2I32 Vec2I32;
union Vec2I32
{
    struct
    {
        I32 x;
        I32 y;
    };
    I32 v[2];
};

typedef union Vec2I16 Vec2I16;
union Vec2I16
{
    struct
    {
        I16 x;
        I16 y;
    };
    I16 v[2];
};

// 3-Vectors ==================================================================

typedef union Vec3F32 Vec3F32;
union Vec3F32
{
    struct
    {
        F32 x;
        F32 y;
        F32 z;
    };
    struct
    {
        Vec2F32 xy;
        F32 _z0;
    };
    struct
    {
        F32 _x0;
        Vec2F32 yz;
    };
    F32 v[3];
};

typedef union Vec3I32 Vec3I32;
union Vec3I32
{
    struct
    {
        I32 x;
        I32 y;
        I32 z;
    };
    struct
    {
        Vec2I32 xy;
        I32 _z0;
    };
    struct
    {
        I32 _x0;
        Vec2I32 yz;
    };
    I32 v[3];
};

// 4-vectors ==================================================================

typedef union Vec4F32 Vec4F32;
union Vec4F32
{
    struct
    {
        F32 x;
        F32 y;
        F32 z;
        F32 w;
    };
    struct
    {
        Vec2F32 xy;
        Vec2F32 zw;
    };
    struct
    {
        Vec3F32 xyz;
        F32 _z0;
    };
    struct
    {
        F32 _x0;
        Vec3F32 yzw;
    };
    F32 v[4];
};

typedef union Vec4I32 Vec4I32;
union Vec4I32
{
    struct
    {
        I32 x;
        I32 y;
        I32 z;
        I32 w;
    };
    struct
    {
        Vec2I32 xy;
        Vec2I32 zw;
    };
    struct
    {
        Vec3I32 xyz;
        I32 _z0;
    };
    struct
    {
        I32 _x0;
        Vec3I32 yzw;
    };
    I32 v[4];
};

// Range Types
//=============================================================================

// 1 Range ====================================================================

typedef union Rng1U32 Rng1U32;
union Rng1U32
{
    struct
    {
        U32 min;
        U32 max;
    };
    U32 v[2];
};

typedef union Rng1I32 Rng1I32;
union Rng1I32
{
    struct
    {
        I32 min;
        I32 max;
    };
    I32 v[2];
};

typedef union Rng1U64 Rng1U64;
union Rng1U64
{
    struct
    {
        U64 min;
        U64 max;
    };
    U64 v[2];
};

typedef union Rng1I64 Rng1I64;
union Rng1I64
{
    struct
    {
        I64 min;
        I64 max;
    };
    I64 v[2];
};

typedef union Rng1F32 Rng1F32;
union Rng1F32
{
    struct
    {
        F32 min;
        F32 max;
    };
    F32 v[2];
};

// 2 Range (Rectangles) =======================================================

typedef union Rng2I16 Rng2I16;
union Rng2I16
{
    struct
    {
        Vec2I16 min;
        Vec2I16 max;
      };
    struct
    {
        Vec2I16 p0;
        Vec2I16 p1;
    };
    struct
    {
        I16 x0;
        I16 y0;
        I16 x1;
        I16 y1;
    };
    Vec2I16 v[2];
};

typedef union Rng2I32 Rng2I32;
union Rng2I32
{
    struct
    {
        Vec2I32 min;
        Vec2I32 max;
    };
    struct
    {
        Vec2I32 p0;
        Vec2I32 p1;
    };
    struct
    {
        I32 x0;
        I32 y0;
        I32 x1;
        I32 y1;
    };
    Vec2I32 v[2];
};

typedef union Rng2F32 Rng2F32;
union Rng2F32
{
    struct
    {
        Vec2F32 min;
        Vec2F32 max;
    };
    struct
    {
        Vec2F32 p0;
        Vec2F32 p1;
    };
    struct
    {
        F32 x0;
        F32 y0;
        F32 x1;
        F32 y1;
    };
    Vec2F32 v[2];
};

typedef union Rng2I64 Rng2I64;
union Rng2I64
{
    struct
    {
        Vec2I64 min;
        Vec2I64 max;
    };
    struct
    {
        Vec2I64 p0;
        Vec2I64 p1;
    };
    struct
    {
        I64 x0;
        I64 y0;
        I64 x1;
        I64 y1;
    };
    Vec2I64 v[2];
};

// Units
//=============================================================================

// Numerical Units
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

// Unit of Information
#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)

// Constants
//=============================================================================

#define MATH_PI           3.14159265358979323846264338327950288f

// Vector Ops
//=============================================================================

// 2-Vectors ==================================================================

internal Vec2I16 vec_2i16(I16 x, I16 y);
internal Vec2I32 vec_2i32(I32 x, I32 y);
internal Vec2I64 vec_2i64(I64 x, I64 y);
internal Vec2F32 vec_2f32(F32 x, F32 y);

// 3-Vectors ==================================================================

internal Vec3I32 vec_3i32(I32 x, I32 y, I32 z);
internal Vec3F32 vec_3f32(F32 x, F32 y, F32 z);

// 4-vectors ==================================================================

internal Vec4I32 vec_4i32(I32 x, I32 y, I32 z, I32 w);
internal Vec4F32 vec_4f32(F32 x, F32 y, F32 z, F32 w);

// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal Rng1I32 rng_1i32(I32 min, I32 max);
internal Rng1I64 rng_1i64(I64 min, I64 max);
internal Rng1U32 rng_1u32(U32 min, U32 max);
internal Rng1U64 rng_1u64(U64 min, U64 max);
internal Rng1F32 rng_1f32(F32 min, F32 max);

internal I32 dim_1i32(Rng1I32 r);
internal I64 dim_1i64(Rng1I64 r);
internal U32 dim_1u32(Rng1U32 r);
internal U64 dim_1u64(Rng1U64 r);
internal F32 dim_1f32(Rng1F32 r);

// 2 Range (Rectangles) =======================================================

internal Rng2I16 rng_2i16(Vec2I16 min, Vec2I16 max);
internal Rng2I32 rng_2i32(Vec2I32 min, Vec2I32 max);
internal Rng2I64 rng_2i64(Vec2I64 min, Vec2I64 max);
internal Rng2F32 rng_2f32(Vec2F32 min, Vec2F32 max);

#define rng_2i16p(x, y, z, w) rng_2i16(vec_2i16((x), (y)), vec_2i16((z), (w)))
#define rng_2i32p(x, y, z, w) rng_2i32(vec_2i32((x), (y)), vec_2i32((z), (w)))
#define rng_2i64p(x, y, z, w) rng_2i64(vec_2i64((x), (y)), vec_2i64((z), (w)))
#define rng_2f32p(x, y, z, w) rng_2f32(vec_2f32((x), (y)), vec_2f32((z), (w)))

internal Vec2I16 dim_2i16(Rng2I16 r);
internal Vec2I32 dim_2i32(Rng2I32 r);
internal Vec2I64 dim_2i64(Rng2I64 r);
internal Vec2F32 dim_2f32(Rng2F32 r);

// Random Number
//=============================================================================

internal U32 math_random_u32(U32 seed);

#endif // BASE_MATH_H
