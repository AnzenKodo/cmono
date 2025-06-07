#include "src/base/base_include.h"
#include "src/os/os_include.h"

#include "src/base/base_include.c"
#include "src/os/os_include.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROJECT_NAME    "Scuttle"
#define PROJECT_VERSION "0.1"
#define BUILD_MAIN_FILE "src/scuttle/scuttle_entry_point.c"
#define BUILD_DIR       "build"

const char *help_message = "build.c: C file that build's C projects.\n"
"Version: "PROJECT_VERSION"\n"
"Options:\n"
"   build                   Build project\n"
"   run                     Run project\n"
"   build-run               Build and Run project\n"
"   build-debugger          Build for Debugger\n"
"   test                    Test project (Requires: valgrid, typos)\n"
"   profiler                Runs Profiler (Requires: perf)\n"
"   version --version -v    Print project version\n"
"   help --help -h          Print help\n";

char cmd[100] = "";

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
    build_cmd_append(cmd, " -lm -lxcb -lXau -lXdmcp -lxcb-image -lX11");
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

internal void entry_point(char *argv[])
{
    // char *cmd = getenv("CMDLINE");
    char *option = argv[1];

    if (!option) {
        fprintf(stderr, "Error: no options provided.\n\n");
        printf(help_message);
    } else if (str8_match(str8_from_cstr(option), str8("build"))) {
        build_compile(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("build-run"))) {
        build_compile(cmd);
        build_run(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("build-debugger"))) {
        build_debugger(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("run"))) {
        build_run(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("test"))) {
        build_test(cmd);
    } else if (str8_match(str8_from_cstr(option), str8("profiler"))) {
        build_profiler(cmd);
    } else if (
        str8_match(str8_from_cstr(option), str8("version")) ||
        str8_match(str8_from_cstr(option), str8("--version")) ||
        str8_match(str8_from_cstr(option), str8("-v"))
    ) {
        printf(PROJECT_VERSION);
    } else if (
        str8_match(str8_from_cstr(option), str8("help")) ||
        str8_match(str8_from_cstr(option), str8("--help")) ||
        str8_match(str8_from_cstr(option), str8("-h"))
    ) {
        printf(help_message);
    } else {
        fprintf(stderr, "Error: wrong option provided `%s`.\n\n", option);
        printf(help_message);
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
	mem_set(cmd, 0, cstr8_length(cmd));
}
