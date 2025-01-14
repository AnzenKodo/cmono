#include "ccore/core.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PROJECT_NAME    "Scuttle"
#define VERSION         "0.1"
#define BUILD_MAIN_FILE "main.c"
#define BUILD_DIR       "build"

const char *help_message = "build.c: A C file that build's C projects.\n"
"Version: "VERSION"\n"
"Options:\n"
"   build                   Build project\n"
"   run                     Run project\n"
"   build-run               Build and Run project\n"
"   version --version -v    Print project version\n"
"   help --help -h          Print help\n";

char cmd[100] = "";

fn void build_cmd_append(char *cmd, char *src);
fn void build_cmd_finish(char *cmd);
fn void build_cmd_run(char *cmd);

fn void build_compile(char *cmd)
{
    if (os_mkdir(BUILD_DIR)) {
        printf("Created `"BUILD_DIR"` directory.\n");
    }

    printf("Compling:\n");
    build_cmd_append(cmd, "cc "BUILD_MAIN_FILE);
    build_cmd_append(cmd, " -o "BUILD_DIR"/"PROJECT_NAME);   // Output
    build_cmd_append(cmd, " -lX11");                         // Libs
    build_cmd_append(cmd, " -ggdb -g3");                     // Debug
    build_cmd_append(cmd, " -Wall -Wextra");                 // Warnings
    build_cmd_append(cmd, " -Wno-unused-variable -Wno-unused-function -Wno-incompatible-pointer -Wno-override-init -Wno-missing-braces -Wno-incompatible-pointer-types"); // Disable useless warnings
    build_cmd_finish(cmd);
}

fn void build_run(char *cmd)
{
    build_cmd_append(cmd, BUILD_DIR"/"PROJECT_NAME);
    printf("Running:\n");
    build_cmd_finish(cmd);
}

int main(int argc, char *argv[])
{
    // char *cmd = getenv("CMDLINE");

    char *option = argv[1];

    if (!option) {
        fprintf(stderr, "Error: no options provided.\n\n");
        printf(help_message);
    } else if (str8_match(str8_cstr(option), str8_lit("build"))) {
        build_compile(cmd);
    } else if (str8_match(str8_cstr(option), str8_lit("build-run"))) {
        build_compile(cmd);
        build_run(cmd);
    } else if (str8_match(str8_cstr(option), str8_lit("run"))) {
        build_run(cmd);
    } else if (
        str8_match(str8_cstr(option), str8_lit("version")) ||
        str8_match(str8_cstr(option), str8_lit("--version")) ||
        str8_match(str8_cstr(option), str8_lit("-v"))
    ) {
        printf(VERSION);
    } else if (
        str8_match(str8_cstr(option), str8_lit("help")) ||
        str8_match(str8_cstr(option), str8_lit("--help")) ||
        str8_match(str8_cstr(option), str8_lit("-h"))
    ) {
        printf(help_message);
    } else {
        fprintf(stderr, "Error: wrong option provided `%s`.\n\n", option);
        printf(help_message);
    }
}

fn void build_cmd_append(char *cmd, char *src)
{
	strcat(cmd, src);
}

fn void build_cmd_run(char *cmd)
{
	printf("%s\n", cmd);
	int err = system(cmd);
	if (err)
	{
		fprintf(stderr, "\nError: %s\n", strerror(err));
		os_exit(err);
	}
}

fn void build_cmd_finish(char *cmd)
{
	build_cmd_run(cmd);
	memset(cmd, 0, strlen(cmd));
}
