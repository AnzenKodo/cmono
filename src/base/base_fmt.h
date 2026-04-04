#ifndef BASE_FMT_H
#define BASE_FMT_H

// External Includes
//=============================================================================

#define STB_SPRINTF_DECORATE(name) fmt_##name
#define STB_SPRINTF_STATIC
#include "./external/stb_sprintf.h"

#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#   define FmtTypeCheck(fmt_idx, arg_idx) __attribute__((format(printf, fmt_idx, arg_idx)))
#elif defined(COMPILER_MSVC)
#   include <sal.h>
#   if _MSC_VER > 1400
#       define FmtTypeCheck(fmt_idx, arg_idx) _Printf_format_string_
#   endif
#else
#   error printf type-safety check is not defined for this compiler.
#endif

// Functions
// ============================================================================

// FPrint =====================================================================

internal uint64_t fmt_fprint(Os_File file, const char *string);
internal uint64_t fmt_fprintln(Os_File file, const char *string);
internal uint64_t fmt_vfprintf(Os_File file, const char *format, va_list args);
internal uint64_t fmt_fprintf(Os_File file, const char *format, ...) FmtTypeCheck(2, 3);
internal uint64_t fmt_vfprintfln(Os_File file, const char *format, va_list args);
internal uint64_t fmt_fprintfln(Os_File file, const char *format, ...) FmtTypeCheck(2, 3);

// Print ======================================================================

internal void fmt_print(const char *string);
internal void fmt_println(const char *string);
internal void fmt_vprintf(const char *format, va_list args);
internal void fmt_printf(const char *format, ...) FmtTypeCheck(1, 2);
internal void fmt_vprintfln(const char *format, va_list args);
internal void fmt_printfln(const char *format, ...) FmtTypeCheck(1, 2);

// EPrint =====================================================================

internal void fmt_eprint(const char *string);
internal void fmt_eprintln(const char *string);
internal void fmt_veprintf(const char *format, va_list args);
internal void fmt_eprintf(const char *format, ...) FmtTypeCheck(1, 2);
internal void fmt_veprintfln(const char *format, va_list args);
internal void fmt_eprintfln(const char *format, ...) FmtTypeCheck(1, 2);

#endif // BASE_FMT_H
