#include "src/base/base_include.h"
#include "src/os/os_include.h"

#include "src/base/base_include.c"
#include "src/os/os_include.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME    "Scuttle"
#define PROJECT_VERSION "0.1"
// #define BUILD_MAIN_FILE "src/scuttle/scuttle_entry_point.c"
#define BUILD_MAIN_FILE "src/shaderplay/shaderplay_entry_point.c"
#define BUILD_DIR       "build"

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

internal void build_compile_cc(char *cmd) {
    build_cmd_append(cmd, "cc "BUILD_MAIN_FILE);
    build_cmd_append(cmd, " -o ./"BUILD_DIR"/"PROJECT_NAME); // Output
    build_cmd_append(cmd, " -ggdb -g3 -DBUILD_DEBUG");       // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                 // Warnings
    // Security
    build_cmd_append(cmd, " -mshstk -fstack-protector -fcf-protection=full");
    // Disable useless warnings in C
    build_cmd_append(cmd, "  -Wno-incompatible-pointer-types -Wno-override-init");
    // Performance
    // build_cmd_append(cmd, " -mavx2 -O3");
    // Libs
    // NOTE(ak): to libs parameters `pkg-config --static --libs xcb`
    build_cmd_append(cmd, " -lm -lxcb -lXau -lXdmcp -lxcb-image -lEGL -lGL");
}

internal void build_compile(char *cmd)
{
    if (os_dir_make(str8(BUILD_DIR))) {
        printf("Created `"BUILD_DIR"` directory.\n");
    }
    printf("Compiling:\n");
    build_compile_cc(cmd);
    // Disable useless warnings
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-missing-braces");
    // build_cmd_append(cmd, " -static");
    build_cmd_append(cmd, " -fsanitize=address");
    build_cmd_finish(cmd);
}

internal void build_run(char *cmd)
{
    build_cmd_append(cmd, "./"BUILD_DIR"/"PROJECT_NAME);
    build_cmd_append(cmd, " shaders/shader.frag");
    printf("Running:\n");
    build_cmd_finish(cmd);
}

internal void build_debugger(char *cmd)
{
    if (os_dir_make(str8(BUILD_DIR))) {
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

char cmd[100] = "";
internal void entry_point()
{
    bool should_print_help = false;
    bool should_print_version = false;
    bool should_program_build = false;
    bool should_program_run = false;
    bool should_program_debug = false;
    bool should_program_test = false;
    bool should_program_profiler = false;

    Str8List *args_list = os_args_get();

    if (args_list->count == 1) {
        should_print_help = true;
    }
    for (Str8Node *node = args_list->first->next; node != NULL; node = node->next) {
        if (
            str8_match(node->string, str8("help"), 0) ||
            str8_match(node->string, str8("--help"), 0) ||
            str8_match(node->string, str8("-h"), 0)
        ) {
            should_print_help = true;
            should_print_version = true;
        } else if (
            str8_match(node->string, str8("version"), 0) ||
            str8_match(node->string, str8("--version"), 0) ||
            str8_match(node->string, str8("-v"), 0)
        ) {
            should_print_version = true;
        } else if (str8_match(node->string, str8("build"), 0)) {
            should_program_build = true;
        } else if (str8_match(node->string, str8("build-run"), 0)) {
            should_program_build = true;
            should_program_run = true;
        } else if (!should_program_test && str8_match(node->string, str8("build-debugger"), 0)) {
            should_program_debug = true;
        } else if (!should_program_debug && str8_match(node->string, str8("test"), 0)) {
            should_program_test = true;
        } else if (str8_match(node->string, str8("run"), 0)) {
            should_program_run = true;
        } else if(!should_program_build && str8_match(node->string, str8("profile"), 0)) {
            should_program_profiler = false;
        } else {
            fprintf(stderr, "Error: wrong option provided `%s`.\n\n", node->string.str);
            should_print_help = true;
            should_print_version = true;
        }
    }
    if (!(should_print_help || should_print_version)) {
        if (should_program_build) {
            if (should_program_debug) {
                build_debugger(cmd);
            } else if (should_program_test) {
                build_test(cmd);
            } else {
                build_compile(cmd);
            }
        }
        if (should_program_run) {
            build_run(cmd);
        }
        if (should_program_profiler) {
            build_profiler(cmd);
        }
    }
    if (should_print_help) {
        printf(help_message);
    }
    if (should_print_version) {
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
