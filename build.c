// Includes
// ============================================================================

#include "src/base/base_include.h"
#include "src/os/os_include.h"
#include "src/base/base_include.c"
#include "src/os/os_include.c"

// External Includes ==========================================================

#include <stdlib.h>
#include <string.h>

// Defines
// ============================================================================

#define BUILD_CMD_SIZE 1024

// Types
// ============================================================================

typedef enum Build_Type
{
    Build_Type_None,
    Build_Type_Dev,
    Build_Type_Debug,
    Build_Type_Profiler,
} Build_Type;

typedef struct Build_Info Build_Info;
struct Build_Info
{
    Context_Os os;
    Str8 name;
    Str8 version;
    Str8 entry_point;
    Str8 dir;
    bool mingw;
    Build_Type type;
    U8 cmd[BUILD_CMD_SIZE];
};

// Globals
// ============================================================================


global const char *help_message = "build.c: C file that build's C projects.\n"
"Options:\n"
"   build                Build project\n"
"   run                  Run project\n"
"   build-run            Build and Run project\n"
"   build-debugger       Build for Debugger\n"
"   test                 Test project (Requires: valgrid, typos)\n"
"   profile              Runs Profiler (Requires: perf)\n"
"   version --version -v Print project version\n"
"   help --help -h       Print help\n";

// Functions
// ============================================================================

internal void build_cmd_append(Build_Info *info, const char *format, ...);
internal void build_cmd_finish(Build_Info *info);
internal void build_cmd_run(Build_Info *info);

internal void build_cmd_append_output(Build_Info *info)
{
    if (info->os == Context_Os_Windows)
    {
        build_cmd_append(info, "%s\\%s_", info->dir.cstr, info->name.cstr);
    }
    else
    {
        build_cmd_append(info, "%s/%s_", info->dir.cstr, info->name.cstr);
    }
    switch (info->type)
    {
        case Build_Type_Dev:
        {
            build_cmd_append(info, "Dev");
        }break;
        case Build_Type_Debug:
        {
            build_cmd_append(info, "Debug");
        }break;
        case Build_Type_Profiler:
        {
            build_cmd_append(info, "Profiler");
        }break;
        default:
        {
            build_cmd_append(info, "");
        }
    }
    if (info->os == Context_Os_Windows)
    {
        build_cmd_append(info, ".exe");
    }
}

// Compilers functions ========================================================

internal void build_compile_msvc(Build_Info *info)
{
    build_cmd_append(info, "setup_x64.bat & cl.exe %s", info->entry_point.cstr);
    // Looks
    build_cmd_append(info, " -nologo -diagnostics:caret");
    // Output
    build_cmd_append(info, " -Fo:%s\\ -Fe:", info->dir.cstr);
    build_cmd_append_output(info);
    // Debug
    build_cmd_append(info, " -Zi -Fd\"%s\\vc140.pbd\" -DBUILD_DEBUG=1", info->dir.cstr);
    // Lock C Version
    build_cmd_append(info, " -std:c11");
    // Optimaization
    build_cmd_append(info,
        " -Od"
        " -Ob1 -wd4710" // Disable inline functions and it's warnings
    ); // Disable
    // Warnings
    build_cmd_append(info, " -W4 -Wall");
    // Disbale uselss warnings
    build_cmd_append(info,
        " -wd4668"                 // For macros magic
        " -wd4464"                 // Warning about '..' in path
        " -wd4310 -wd4146 -wd4245" // Cast conversion
        " -wd4201"                 // Nameless struct/union
        " -wd4820"                 // Struct padding
        " -wd4061"                 // Enum switch enumeratio
        " -wd4189"                 // Unused variables
    );
    // Security
    build_cmd_append(info,
        " -Qspectre -wd5045"  // Spectre variant 1 vulnerability
        " -GS"                // Canary insertion
        " -guard:cf"          // Control-flow protection
        // " -fsanitize=address" // AddressSanitizer
    );
}

internal void build_compile_gcc(Build_Info *info)
{
    if (info->mingw)
    {
        build_cmd_append(info, "x86_64-w64-mingw32-gcc");
    }
    else
    {
        build_cmd_append(info, "gcc");
    }
    build_cmd_append(info, " %s", info->entry_point.cstr);
    // Output =================================================================
    build_cmd_append(info, " -o ");
    build_cmd_append_output(info);
    // Lock C Version
    build_cmd_append(info, " -std=gnu99");
    // Debug
    build_cmd_append(info, " -ggdb -g3 -DBUILD_DEBUG");
    // Warning
    build_cmd_append(info, " -Wall -Wextra");
    // Disable useless warnings in C
    build_cmd_append(info,
        " -Wno-unknown-pragmas"
        " -Wno-missing-braces"
        " -Wno-unused-function"
        " -Wno-unused-variable"
    );
    // Security ===============================================================
    build_cmd_append(info, " -mshstk -fcf-protection=full");
    if (info->type != Build_Type_Debug && !info->mingw)
    {
        build_cmd_append(info, " -fstack-protector -fsanitize=address");
    }
    // Libs ===================================================================
    if (info->mingw || info->os == Context_Os_Windows)
    {
        build_cmd_append(info, " -lopengl32 -luser32 -lgdi32");
    }
    else
    {
        build_cmd_append(info, " -lm -lxcb -lXau -lXdmcp -lxcb-image -lEGL -lGL");
    }
}

// Build types functions ======================================================

internal void build_compile(Build_Info *info)
{
    if (os_dir_make(info->dir))
    {
        fmt_printf("Created `%s` directory.\n", info->dir.cstr);
    }
    fmt_println("# Compile --------------------------------------------------------------------#\n");
    if (info->os == Context_Os_Linux)
    {
        build_compile_gcc(info);
    }
    else if (info->os == Context_Os_Windows)
    {
        build_compile_msvc(info);
        // build_compile_gcc(info);
    }
    else
    {
        fmt_printf("Error: OS build compile is not supported.");
    }
    build_cmd_finish(info);
}

internal void build_test(Build_Info *info)
{
    fmt_printf("Compiling:\n");
    build_compile_gcc(info);
    build_cmd_append(info, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces");
    build_cmd_finish(info);
    fmt_printf("Test Typos:\n");
    build_cmd_append(info, "typos");
    build_cmd_finish(info);
    fmt_printf("Test Memory Leaks:\n");
    build_cmd_append(info, "valgrind ");
    build_cmd_append(info, " --leak-check=full --track-origins=yes ");
    build_cmd_append(info, " ./%s/%s", info->dir.cstr, info->name.cstr); // Output
    build_cmd_append(info, " --leak-check=full --show-leak-kinds=all");
    build_cmd_finish(info);
}

internal void build_profiler(Build_Info *info)
{
    fmt_printf("Compiling:\n");
    build_compile_gcc(info);
    build_cmd_append(info, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces");
    build_cmd_finish(info);
    fmt_printf("Profiler Recording:\n");
    build_cmd_append(
        info, "perf record -o ./%s/perf.data -g ./%s/%s",
        info->dir.cstr, info->dir.cstr, info->name.cstr
    );
    build_cmd_finish(info);
    fmt_printf("Profiler Report:\n");
    build_cmd_append(info, "perf report -i ./%s/perf.data", info->dir.cstr);
    build_cmd_finish(info);
}

// Build run functions ========================================================

internal void build_run(Build_Info *info)
{
    fmt_println("# Running --------------------------------------------------------------------#\n");
    if (info->mingw)
    {
        build_cmd_append(info, "WINEARCH=win64 wine ");
    }
    build_cmd_append_output(info);

    if (info->os == Context_Os_Windows)
    {
        build_cmd_append(info, " shaders\\shader.frag");
    }
    else
    {
        build_cmd_append(info, " shaders/shader.frag");
    }

    build_cmd_finish(info);
}

internal void entry_point()
{
    Build_Info info = ZERO_STRUCT;
    info.name = str8("Scuttle");
    info.version = str8("0.2");
    info.entry_point = str8("src/shaderplay/shaderplay_entry_point.c");
    info.dir = str8("build");
    info.os = context_of_os();

    bool should_print_help = false;
    bool should_print_version = false;
    bool build_run_program = false;

    Str8Array *args = os_args_get();

    if (args->length == 1)
    {
        should_print_help = true;
    }

    Str8 arg_str = args->strings[1];
    if (str8_match(arg_str, str8("help"), 0) || str8_match(arg_str,
        str8("--help"), 0) || str8_match(arg_str, str8("-h"), 0))
    {
        should_print_help = true;
        should_print_version = true;
    }
    else if (str8_match(arg_str, str8("version"), 0) ||
        str8_match(arg_str, str8("--version"), 0) ||
        str8_match(arg_str, str8("-v"), 0))
    {
        should_print_version = true;
    }
    else if (str8_match(arg_str, str8("build"), 0))
    {
        info.type = Build_Type_Dev;
    }
    else if (str8_match(arg_str, str8("build-run"), 0))
    {
        info.type = Build_Type_Dev;
        build_run_program = true;
    }
    else if (str8_match(arg_str, str8("build-debugger"), 0))
    {
        info.type = Build_Type_Debug;
    }
    else if (str8_match(arg_str, str8("run"), 0))
    {
        build_run_program = true;
    } else if(str8_match(arg_str, str8("profile"), 0)) {
        info.type = Build_Type_Profiler;
    }
    else
    {
        fmt_eprintf("Error: wrong option provided `%s`.\n\n", arg_str.cstr);
        should_print_help = true;
        should_print_version = true;
    }
    if (str8_match(args->strings[2], str8("mingw"), 0))
    {
        info.mingw = true;
    }
    fmt_println("#=============================================================================#");
    fmt_println("# Build Output                                                                #");
    fmt_println("#=============================================================================#\n");
    if (!(should_print_help || should_print_version))
    {
        if (info.type != Build_Type_None)
        {
            build_compile(&info);
        }
        if (build_run_program)
        {
            build_run(&info);
        }
    }
    if (should_print_help)
    {
        fmt_printf("%s", help_message);
    }
    if (should_print_version)
    {
        fmt_printf("Version: %s", info.dir.cstr);
    }
}

internal void build_cmd_append(Build_Info *info, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    {
        char buffer[BUILD_CMD_SIZE];
        fmt_vsnprintf(buffer, sizeof(buffer), format, args);
        strcat(Cast(char *)info->cmd, buffer);
    }
    va_end(args);
}

internal void build_cmd_run(Build_Info *info)
{
    fmt_printf("Command: %s\n\n", info->cmd);
    int err = system((const char *)info->cmd);
    if (err)
    {
        fmt_eprintf("\nError: %s\n", strerror(err));
        os_exit(err);
    }
}

internal void build_cmd_finish(Build_Info *info)
{
    build_cmd_run(info);
    mem_set(info->cmd, 0, cstr8_length(Cast(U8 *)info->cmd));
}
