#include "src/base/base_include.h"
#include "src/os/os_include.h"

#include "src/base/base_include.c"
#include "src/os/os_include.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum Build_Type
{
    Build_Type_Debug;
    Build_Type_Test;
    Build_Type_Profiler;
} Build_Type;

typedef struct Build_Info Build_Info;
struct Build_Info
{
    Context_Os os;
    String name;
    String version;
    String entry_point;
    String dir;
    bool mingw;
    Build_Type type;
} Build_Info;

global Build_Info build_info = ZERO_STRUCT;

const char *help_message = "build.c: C file that build's C projects.\n"
"Options:\n"
"   build                Build project\n"
"   run                  Run project\n"
"   build-run            Build and Run project\n"
"   build-debugger       Build for Debugger\n"
"   test                 Test project (Requires: valgrid, typos)\n"
"   profile              Runs Profiler (Requires: perf)\n"
"   version --version -v Print project version\n"
"   help --help -h       Print help\n";


internal void build_cmd_append(char *cmd, char *src);
internal void build_cmd_finish(char *cmd);
internal void build_cmd_run(char *cmd);

internal void build_compile_cl(char *cmd)
{
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME".exe"); // Output
    build_cmd_append(cmd, " -Wl,/subsystem:windows -Wl,/ENTRY:mainCRTStartup");
}

internal void build_compile_mingw(char *cmd)
{
    build_cmd_append(cmd, "x86_64-w64-mingw32-gcc ");
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME".exe"); // Output
    build_cmd_append(cmd, " -ggdb -g3 -DBUILD_DEBUG");             // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                       // Warnings
    build_cmd_append(cmd, " -mshstk -fcf-protection=full");
    // Disable useless warnings in C
    build_cmd_append(cmd, "  -Wno-incompatible-pointer-types -Wno-override-init -Wno-implicit-fallthrough");
    build_cmd_append(cmd, " -lgdi32 -luser32 -lopengl32");
}

internal void build_compile_cc(char *cmd)
{
    printf("Compiling:\n");
    build_cmd_append(cmd, "cc ");
    build_cmd_append(cmd, BUILD_MAIN_FILE);
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME); // Output
    build_cmd_append(cmd, " -ggdb -g3 -DBUILD_DEBUG");       // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                 // Warnings
    build_cmd_append(cmd, " -fstack-protector");
    build_cmd_append(cmd, " -mshstk -fcf-protection=full");
    // Disable useless warnings in C
    build_cmd_append(cmd, "  -Wno-incompatible-pointer-types -Wno-override-init" 
        " -Wno-implicit-fallthrough -Wno-unused-variable -Wno-unused-parameter" 
        " -Wno-unused-function -Wno-unused-but-set-variable -Wno-missing-braces");
    // Libs
    // NOTE(ak): to libs parameters `pkg-config --static --libs xcb`
    build_cmd_append(cmd, " -lm -lxcb -lXau -lXdmcp -lxcb-image -lEGL -lGL");
    build_cmd_append(cmd, " -lgdi32 -luser32 -lopengl32");
    build_cmd_append(cmd, " -fsanitize=address");
}

internal void build_compile(char *cmd)
{
    if (os_dir_make(str8(BUILD_DIR)))
    {
        printf("Created `"BUILD_DIR"` directory.\n");
    }
    Context_Os os = context_of_os();
    if (os == Os_Linux && !build_use_mingw)
    {
        build_compile_cc(cmd);
    } 
    else if (os == Os_Windows)
    {
        build_compile_cl(cmd);
    } 
    else if (build_use_mingw) 
    {
        build_compile_mingw(cmd);
    }
    else
    {
        printf("Error: OS build compile is not supported.");
    }
    build_cmd_finish(cmd);
}

internal void build_run_windows(char *cmd)
{
    build_cmd_append(cmd, BUILD_DIR"\\"PROJECT_NAME".exe");
    build_cmd_append(cmd, " shaders\\shader.frag");
}

internal void build_run_wine(char *cmd)
{
    build_cmd_append(cmd, "WINEARCH=win64 wine ./"BUILD_DIR"/"PROJECT_NAME".exe");
    build_cmd_append(cmd, " shaders/shader.frag");
}

internal void build_run_linux(char *cmd)
{
    build_cmd_append(cmd, "./"BUILD_DIR"/"PROJECT_NAME);
    build_cmd_append(cmd, " shaders/shader.frag");
}

internal void build_run(char *cmd)
{
    printf("Running:\n");
    Context_Os os = context_of_os();
    if (os == Os_Linux && !build_use_mingw)
    {
        build_run_linux(cmd);
    } 
    else if (os == Os_Windows)
    {
        build_run_windows(cmd);
    } 
    else if (build_use_mingw) 
    {
        build_run_wine(cmd);
    }
    else
    {
        printf("Error: OS build run is not supported.");
    }
    build_cmd_finish(cmd);
}

internal void build_debugger(char *cmd)
{
    if (os_dir_make(str8(BUILD_DIR)))
    {
        printf("Created `"BUILD_DIR"` directory.\n");
    }
    printf("Compiling:\n");
    build_compile_cc(cmd);
    // Disable useless warnings
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-missing-braces");
    // build_cmd_append(cmd, " -static");
    build_cmd_finish(cmd);
}

internal void build_test(char *cmd)
{
    printf("Compiling:\n");
    build_compile_cc(cmd);
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces");
    build_cmd_finish(cmd);
    printf("Test Typos:\n");
    build_cmd_append(cmd, "typos");
    build_cmd_finish(cmd);
    printf("Test Memory Leaks:\n");
    build_cmd_append(cmd, "valgrind ");
    build_cmd_append(cmd, " --leak-check=full --track-origins=yes ");
    build_cmd_append(cmd, "./"BUILD_DIR"/"PROJECT_NAME);
    build_cmd_append(cmd, " --leak-check=full --show-leak-kinds=all");
    build_cmd_finish(cmd);
}

internal void build_profiler(char *cmd)
{
    printf("Compiling:\n");
    build_compile_cc(cmd);
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-missing-braces");
    build_cmd_finish(cmd);
    printf("Profiler Recording:\n");
    build_cmd_append(cmd, "perf record -o ./"BUILD_DIR"/perf.data -g ./"BUILD_DIR"/"PROJECT_NAME);
    build_cmd_finish(cmd);
    printf("Profiler Report:\n");
    build_cmd_append(cmd, "perf report -i ./"BUILD_DIR"/perf.data");
    build_cmd_finish(cmd);
}

char cmd[500] = "";
internal void entry_point()
{
    build_info.name = str8("Scuttle");
    build_info.version = "0.2";
    build_info.entry_point = "src/shaderplay/shaderplay_entry_point.c";
    build_info.dir = "build";

    bool should_print_help = false;
    bool should_print_version = false;
    bool should_program_build = false;
    bool should_program_run = false;

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
        should_program_build = true;
    }
    else if (str8_match(arg_str, str8("build-run"), 0))
    {
        should_program_build = true;
        should_program_run = true;
    }
    else if (!should_program_test && str8_match(arg_str, str8("build-debugger"), 0))
    {
        should_program_debug = true;
    } else if (!should_program_debug && str8_match(arg_str, str8("test"), 0))
    {
        should_program_test = true;
    }
    else if (str8_match(arg_str, str8("run"), 0))
    {
        should_program_run = true;
    }
    else if(!should_program_build && str8_match(arg_str, str8("profile"), 0))
    {
        should_program_profiler = false;
    }
    else
    {
        fprintf(stderr, "Error: wrong option provided `%s`.\n\n", arg_str.cstr);
        should_print_help = true;
        should_print_version = true;
    }
    if (str8_match(args->strings[2], str8("mingw"), 0))
    {
        build_use_mingw = true;
    }

    if (!(should_print_help || should_print_version))
    {
        if (should_program_build) {
            if (should_program_debug)
            {
                build_debugger(cmd);
            }
            else if (should_program_test)
            {
                build_test(cmd);
            }
            else
            {
                build_compile(cmd);
            }
        }
        if (should_program_run)
        {
            build_run(cmd);
        }
        if (should_program_profiler)
        {
            build_profiler(cmd);
        }
    }
    if (should_print_help)
    {
        printf("%s", help_message);
    }
    if (should_print_version)
    {
        printf("Version: "PROJECT_VERSION);
    }
}

internal void build_cmd_append(char *cmd, char *src)
{
	strcat(cmd, src);
}

internal void build_cmd_run(char *cmd)
{
    printf("%s\n\n", cmd);
    int err = system(cmd);
    if (err)
    {
        fprintf(stderr, "\nError: %s\n", strerror(err));
        os_exit(err);
    }
}

internal void build_cmd_finish(char *cmd)
{
    build_cmd_run(cmd);
    mem_set(cmd, 0, cstr8_length(cast(U8 *)cmd));
}
