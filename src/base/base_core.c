// Safe Casts
//=============================================================================

internal U16
safe_cast_u16(U32 x)
{
    AssertAlways(x <= max_u16);
    U16 result = (U16)x;
    return result;
}

internal U32
safe_cast_u32(U64 x)
{
    AssertAlways(x <= max_u32);
    U32 result = (U32)x;
    return result;
}

internal I32
safe_cast_s32(I64 x)
{
    AssertAlways(x <= max_i32);
    I32 result = (I32)x;
    return result;
}
// Toolchain/Environment Enum Functions
//=============================================================================

internal Context_Os
context_of_os(void)
{
    Context_Os os = Context_Os_Null;
#if OS_WINDOWS
    os = Context_Os_Windows;
#elif OS_LINUX
    os = Context_Os_Linux;
#elif OS_MAC
    os = Context_Os_Mac;
#endif
    return os;
}

internal Context_Arch
context_of_arch(void){
    Context_Arch arch = Context_Arch_Null;
#if ARCH_X64
    arch = Context_Arch_X64;
#elif ARCH_X86
    arch = Context_Arch_X86;
#elif ARCH_ARM64
    arch = Context_Arch_Arm64;
#elif ARCH_ARM32
    arch = Context_Arch_Arm32;
#endif
    return arch;
}

internal Context_Compiler
context_of_compiler(void){
    Context_Compiler compiler = Context_Compiler_Null;
#if COMPILER_MSVC
    compiler = Context_Compiler_msvc;
#elif COMPILER_GCC
    compiler = Context_Compiler_gcc;
#elif COMPILER_CLANG
    compiler = Context_Compiler_clang;
#endif
    return compiler;
}

internal I32 round_f32_to_i32(F32 a) { I32 result = (I32)(a + 0.5f); return result; }
internal U32 round_f32_to_u32(F32 a) { U32 result = (U32)(a + 0.5f); return result; }
