#ifndef BASE_FMT_H
#define BASE_FMT_H

// External Includes
//=============================================================================

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_DECORATE(name) fmt_##name
#define STB_SPRINTF_STATIC
#include "../external/stb_sprintf.h"

// Functions
//=============================================================================

internal U64 fmt_vsprintfln(char *buffer, const char *format, va_list args);
internal U64 fmt_sprintfln(void *buffer, const char *format, ...);

internal U64 fmt_fprint(Os_File file, const char *string);
internal U64 vfmt_fprintf(Os_File file, const char *format, va_list args);
internal U64 fmt_fprintf(Os_File file, const char *format, ...);
internal U64 vfmt_fprintfln(Os_File file, const char *format, va_list args);
internal U64 fmt_fprintfln(Os_File file, const char *format, ...);

internal void fmt_print(const char *string);
internal void vfmt_printf(const char *format, va_list args);
internal void fmt_printf(const char *format, ...);
internal void vfmt_printfln(const char *format, va_list args);
internal void fmt_printfln(const char *format, ...);

internal void fmt_eprint(const char *string);
internal void vfmt_eprintf(const char *format, va_list args);
internal void fmt_eprintf(const char *format, ...);
internal void vfmt_eprintfln(const char *format, va_list args);
internal void fmt_eprintfln(const char *format, ...);

#endif // BASE_FMT_H
