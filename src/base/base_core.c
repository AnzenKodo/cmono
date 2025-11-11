// Safe Casts
//=============================================================================

internal U16 safe_cast_u16(U32 x)
{
    AssertAlways(x <= max_u16);
    U16 result = (U16)x;
    return result;
}

internal U32 safe_cast_u32(U64 x)
{
    AssertAlways(x <= max_u32);
    U32 result = (U32)x;
    return result;
}

internal I32 safe_cast_s32(I64 x)
{
    AssertAlways(x <= max_i32);
    I32 result = (I32)x;
    return result;
}

internal U32
u32_from_u64_saturate(U64 x)
{
  U32 x32 = (x > max_u32)?max_u32:(U32)x;
  return(x32);
}

internal I32 round_f32_to_i32(F32 a) {
    I32 result = (I32)(a + 0.5f); return result;
}
internal U32 round_f32_to_u32(F32 a) {
    U32 result = (U32)(a + 0.5f); return result;
}
