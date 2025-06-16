// Vector Ops
//=============================================================================

// 2-Vectors ==================================================================

internal Vec2F32 vec2f32(F32 x, F32 y)
{
    Vec2F32 v = {x, y};
    return v;
}

internal Vec2I64 vec2i64(I64 x, I64 y)
{
    Vec2I64 v = {x, y};
    return v;
}

internal Vec2I32 vec2i32(I32 x, I32 y)
{
    Vec2I32 v = {x, y};
    return v;
}

internal Vec2I16 vec2i16(I16 x, I16 y)
{
    Vec2I16 v = {x, y};
    return v;
}

// 3-Vectors ==================================================================

internal Vec3F32 vec3f32(F32 x, F32 y, F32 z)
{
    Vec3F32 v = {x, y, z};
    return v;
}

internal Vec3I32 vec3i32(I32 x, I32 y, I32 z)
{
    Vec3I32 v = {x, y, z};
    return v;
}


// 4-vectors ==================================================================

internal Vec4F32 vec4f32(F32 x, F32 y, F32 z, F32 w)
{
    Vec4F32 v = {x, y, z, w};
    return v;
}
internal Vec4I32 vec4i32(I32 x, I32 y, I32 z, I32 w)
{
    Vec4I32 v = {x, y, z, w};
    return v;
}

// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal U32
dim_1u32(Rng1U32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal Rng1U32
rng_1u32(U32 min, U32 max)
{
    Rng1U32 r = {min, max};
    if (r.min > r.max) {
        Swap(U32, r.min, r.max);
    }
    return r;
}

internal I32
dim_1i32(Rng1I32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal Rng1I32 rng_1s32(I32 min, I32 max) {
    Rng1I32 r = {min, max};
    if(r.min > r.max) {
        Swap(I32, r.min, r.max);
    }
    return r;
}

internal U64
dim_1u64(Rng1U64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal Rng1U64
rng_1u64(U64 min, U64 max)
{
    Rng1U64 r = {min, max};
    if(r.min > r.max) {
        Swap(U64, r.min, r.max);
    }
    return r;
}


internal I64
dim_1i64(Rng1I64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal Rng1I64
rng_1s64(I64 min, I64 max)
{
    Rng1I64 r = {min, max};
    if (r.min > r.max) {
        Swap(I64, r.min, r.max);
    }
    return r;
}

internal F32
dim_1f32(Rng1F32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal Rng1F32 rng_1f32(F32 min, F32 max) {
    Rng1F32 r = {min, max};
    if(r.min > r.max) {
        Swap(F32, r.min, r.max);
    }
    return r;
}

// 2 Range (Rectangles) =======================================================

internal Vec2I16
dim_2i16(Rng2I16 r)
{
    return (Vec2I16){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Rng2I16 rng_2s16(Vec2I16 min, Vec2I16 max) {
    return (Rng2I16){min, max};
}

internal Vec2I32
dim_2i32(Rng2I32 r)
{
    return (Vec2I32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Rng2I32 rng_2s32(Vec2I32 min, Vec2I32 max) {
    return (Rng2I32){min, max};
}

internal Vec2I64
dim_2s64(Rng2I64 r)
{
    return (Vec2I64){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0
    };
}
internal Rng2I64 rng_2s64(Vec2I64 min, Vec2I64 max) {
    return (Rng2I64){min, max};
}

internal Vec2F32
dim_2f32(Rng2F32 r)
{
    return (Vec2F32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Rng2F32 rng_2f32(Vec2F32 min, Vec2F32 max) {
    return (Rng2F32){min, max};
}

// Random Number
//=============================================================================

internal U32 math_random_u32(U32 seed)
{
    seed = (seed << 13) ^ seed;
    return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
}
