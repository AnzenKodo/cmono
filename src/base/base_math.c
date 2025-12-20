// Vector Ops
//=============================================================================

// 2-Vectors ==================================================================

internal Vec2_F32 vec2_f32(float x, float y)
{
    Vec2_F32 v = {x, y};
    return v;
}

internal Vec2_I64 vec2_i64(int64_t x, int64_t y)
{
    Vec2_I64 v = {x, y};
    return v;
}

internal Vec2_I32 vec2_i32(int32_t x, int32_t y)
{
    Vec2_I32 v = {x, y};
    return v;
}

internal Vec2_I16 vec2_i16(int16_t x, int16_t y)
{
    Vec2_I16 v = {x, y};
    return v;
}

// 3-Vectors ==================================================================

internal Vec3_F32 vec3_f32(float x, float y, float z)
{
    Vec3_F32 v = {x, y, z};
    return v;
}
internal Vec3_I32 vec3_i32(int32_t x, int32_t y, int32_t z)
{
    Vec3_I32 v = {x, y, z};
    return v;
}


// 4-vectors ==================================================================

internal Vec4_F32 vec4_f32(float x, float y, float z, float w)
{
    Vec4_F32 v = {x, y, z, w};
    return v;
}
internal Vec4_I32 vec4_i32(int32_t x, int32_t y, int32_t z, int32_t w)
{
    Vec4_I32 v = {x, y, z, w};
    return v;
}

// Range Ops
//=============================================================================

// 1 Range ====================================================================

internal Rng1_I32 rng1_i32(int32_t min, int32_t max)
{
    Rng1_I32 r = {min, max};
    if(r.min > r.max) {
        Swap(int32_t, r.min, r.max);
    }
    return r;
}
internal Rng1_I64 rng1_i64(int64_t min, int64_t max)
{
    Rng1_I64 r = {min, max};
    if (r.min > r.max) {
        Swap(int64_t, r.min, r.max);
    }
    return r;
}
internal Rng1_U64 rng1_u64(uint64_t min, uint64_t max)
{
    Rng1_U64 r = {min, max};
    if(r.min > r.max) {
        Swap(uint64_t, r.min, r.max);
    }
    return r;
}
internal Rng1_F32 rng1_f32(float min, float max)
{
    Rng1_F32 r = {min, max};
    if (r.min > r.max) {
        Swap(float, r.min, r.max);
    }
    return r;
}
internal Rng1_U32 rng1_u32(uint32_t min, uint32_t max)
{
    Rng1_U32 r = {min, max};
    if (r.min > r.max) {
        Swap(uint32_t, r.min, r.max);
    }
    return r;
}

internal uint32_t dim1_u32(Rng1_U32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal int32_t dim1_i32(Rng1_I32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal uint64_t dim1_u64(Rng1_U64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal int64_t dim1_i64(Rng1_I64 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}
internal float dim1_f32(Rng1_F32 r)
{
    return (r.max > r.min) ? (r.max - r.min) : 0;
}

// 2 Range (Rectangles) =======================================================

internal Rng2_I16 rng2_i16(Vec2_I16 min, Vec2_I16 max)
{
    return (Rng2_I16){min, max};
}
internal Rng2_I32 rng2_i32(Vec2_I32 min, Vec2_I32 max)
{
    return (Rng2_I32){min, max};
}
internal Rng2_I64 rng2_i64(Vec2_I64 min, Vec2_I64 max)
{
    return (Rng2_I64){min, max};
}
internal Rng2_F32 rng2_f32(Vec2_F32 min, Vec2_F32 max)
{
    return (Rng2_F32){min, max};
}

internal Vec2_I16 dim2_i16(Rng2_I16 r)
{
    return (Vec2_I16){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Vec2_I32 dim2_i32(Rng2_I32 r)
{
    return (Vec2_I32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}
internal Vec2_I64 dim2_i64(Rng2_I64 r)
{
    return (Vec2_I64){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0
    };
}
internal Vec2_F32 dim2_f32(Rng2_F32 r)
{
    return (Vec2_F32){
        (r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0,
        (r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0,
    };
}

// Random Number
//=============================================================================

internal uint32_t math_random_u32(uint32_t seed) 
{
    seed = (seed << 13) ^ seed;
    return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
}
