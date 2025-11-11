// Includes
// ============================================================================

#include "src/base/base_include.h"
#include "src/os/os_include.h"
#include "src/base/base_include.c"
#include "src/os/os_include.c"
#include "src/shaderplay/shaderplay_core.h"

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
    Build_Type_Release,
} Build_Type;

typedef struct Build_Info Build_Info;
struct Build_Info
{
    Context_Os os;
    Str8 name;
    Str8 cmd_name;
    Str8 version;
    Str8 entry_point;
    Str8 dir;
    bool mingw;
    Build_Type type;
    U8 cmd[BUILD_CMD_SIZE];
    Log_Config log_config;
};

// Globals
// ============================================================================


global const char *help_message = "build.c: C file that build's C projects.\n"
"USAGE:\n"
"   build.c [OPTIONS]\n"
"OPTIONS:\n"
"   build                Build project\n"
"   run                  Run project\n"
"   build-run            Build and Run project\n"
"   build-debugger       Build for Debugger\n"
"   test                 Test project (Requires: valgrid, typos)\n"
"   profile              Runs Profiler (Requires: perf)\n"
"   --help -h       Print help\n";

// Functions
// ============================================================================

internal void build_cmd_append(Build_Info *info, const char *format, ...);
internal void build_cmd_finish(Build_Info *info);
internal void build_cmd_run(Build_Info *info);
internal char *build_type_to_str8(Build_Info *info);

internal void build_cmd_append_output(Build_Info *info)
{
    if (info->os == Context_Os_Windows)
    {
        build_cmd_append(info, "%s\\%s", info->dir.cstr, info->cmd_name.cstr);
    }
    else
    {
        build_cmd_append(info, "%s/%s", info->dir.cstr, info->cmd_name.cstr);
    }
    if (info->type != Build_Type_Release)
    {
        build_cmd_append(info, "_%s", build_type_to_str8(info));
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
    if (info->type != Build_Type_Release)
    {
        build_cmd_append(info, " -Zi -Fd\"%s\\vc140.pbd\" -DBUILD_DEBUG=1", info->dir.cstr);
    }
    // Lock C Version
    build_cmd_append(info, " -std:c11");
    // Optimaization
    if (info->type == Build_Type_Release)
    {
        build_cmd_append(info, " -Ox -wd4711"); // Enable
    }
    else
    {
        build_cmd_append(info,
            " -Od"
            " -Ob1 -wd4710" // Disable inline functions and it's warnings
        ); // Disable
    }
    // Warnings
    build_cmd_append(info, " -W4 -Wall");
    // Disbale uselss warnings
    build_cmd_append(info,
        " -wd4668"                 // For macros magic
        " -wd4464"                 // Warning about '..' in path
        " -wd4310 -wd4146 -wd4245" // Cast conversion
        " -wd4201"                 // Nameless struct/union
        " -wd4820"                 // Struct padding
        " -wd4061"                 // Enum switch enumeration
    );
    // Security
    build_cmd_append(info,
        " -Qspectre -wd5045"  // Spectre variant 1 vulnerability
        " -GS"                // Canary insertion
        " -guard:cf"          // Control-flow protection
    );
    if (info->type != Build_Type_Debug || info->type != Build_Type_Release)
    {
        // build_cmd_append(info, " -fsanitize=address"); // AddressSanitizer
    }
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
    // Optimaization
    if (info->type == Build_Type_Release)
    {
        build_cmd_append(info, " -O3"); // Enable
    }
    if (info->type == Build_Type_Debug)
    {
        build_cmd_append(info, " -Og"); // Enable Debug friendly optimaization
    }
    else
    {
        build_cmd_append(info, " -O0");
    }
    // Debug
    if (info->type != Build_Type_Release)
    {
        build_cmd_append(info, " -ggdb -g3 -DBUILD_DEBUG");
    }
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
        build_cmd_append(info, " -fstack-protector -fsanitize=address,undefined -fno-omit-frame-pointer");
        // build_cmd_append(info, "-fanalyzer");
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
    fmt_println("# Compile --------------------------------------------------------------------#");
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
    build_cmd_append(info, " ./%s/%s", info->dir.cstr, info->cmd_name.cstr); // Output
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
        info->dir.cstr, info->dir.cstr, info->cmd_name.cstr
    );
    build_cmd_finish(info);
    fmt_printf("Profiler Report:\n");
    build_cmd_append(info, "perf report -i ./%s/perf.data", info->dir.cstr);
    build_cmd_finish(info);
}

// Build run functions ========================================================

internal void build_run(Build_Info *info)
{
    fmt_println("# Running --------------------------------------------------------------------#");
    if (info->mingw)
    {
        build_cmd_append(info, "WINEARCH=win64 wine ");
    }
    build_cmd_append_output(info);
    if (info->os == Context_Os_Windows)
    {
        build_cmd_append(info, " shaders\\circles.frag");
    }
    else
    {
        build_cmd_append(info, " shaders/circles.frag");
    }
    build_cmd_finish(info);
}

internal void entry_point()
{
    Build_Info info = ZERO_STRUCT;
    info.name = str8(PROGRAM_NAME);
    info.cmd_name = str8(PROGRAM_CMD_NAME);
    info.entry_point = str8("src/shaderplay/shaderplay_entry_point.c");
    info.dir = str8("build");
    info.os = Context_Os_CURRENT;
    info.log_config = log_init();

    bool should_print_help = false;
    bool build_run_program = false;

    Flags_Context context = flags_init();
    Str8 name = ZERO_STRUCT;
    flags_string(&context, str8("name"), &name, str8("a"), str8("Name of the program"));
    Str8Array *args = os_args_get();
    if (!flags_parse(&context, args))
    {
        flags_print_error(&context);
        os_exit(1);
    }
    fmt_println((const char *)name.cstr);

    return;
    if (args->length >= 2)
    {
        Str8 arg1 = args->strings[1];
        Str8 arg2 = ZERO_STRUCT;
        if (args->length == 3)
        {
            arg2 = args->strings[2];
        }
        if (str8_match(arg1, str8("--help"), 0) || str8_match(arg1, str8("-h"), 0))
        {
            should_print_help = true;
        }
        else if (str8_match(arg1, str8("build"), 0))
        {
            if (str8_match(arg2, str8("release"), 0))
            {
                info.type = Build_Type_Release;
            }
            else
            {
                info.type = Build_Type_Dev;
            }
        }
        else if (str8_match(arg1, str8("build-run"), 0))
        {
            if (str8_match(arg2, str8("release"), 0))
            {
                info.type = Build_Type_Release;
            }
            else
            {
                info.type = Build_Type_Dev;
            }
            build_run_program = true;
        }
        else if (str8_match(arg1, str8("build-debugger"), 0))
        {
            info.type = Build_Type_Debug;
        }
        else if (str8_match(arg1, str8("run"), 0))
        {
            build_run_program = true;
        } else if(str8_match(arg1, str8("profile"), 0)) {
            info.type = Build_Type_Profiler;
        }
        else
        {
            fmt_eprintf("Error: wrong option provided `%s`.\n\n", arg1.cstr);
            should_print_help = true;
            os_exit(1);
        }
    } else {
        should_print_help = true;
    }
    if (str8_match(args->strings[2], str8("mingw"), 0))
    {
        info.mingw = true;
    }
    for (U32 i = 0; i < args->length; i++)
    {
        if (str8_match(args->strings[i], str8("--nocolor"), 0)) {
            info.log_config.color_log = false;
        }
    }
    if (!(should_print_help))
    {
        fmt_println("# Build Output ===============================================================#");
        fmt_printfln("Build Type: %s", build_type_to_str8(&info));
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
    fmt_printf("Command: %s\n", info->cmd);
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

internal char *build_type_to_str8(Build_Info *info)
{
    char *type_string;
    switch (info->type)
    {
        case Build_Type_Dev:
        {
            type_string = "dev";
        }
        break;
        case Build_Type_Debug:
        {
            type_string = "debug";
        }
        break;
        case Build_Type_Profiler:
        {
            type_string = "profiler";
        }
        break;
        case Build_Type_Release:
        {
            type_string = "release";
        }
        break;
        default:
        {
            type_string = "";
        }
    }
    return type_string;
}
