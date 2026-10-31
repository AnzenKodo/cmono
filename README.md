# CMono

A C monorepo for my personal projects

## Dependences

- Required
    - C Compiler: clang & gcc
    - In Linux:
        - Libraries:
            - C Math Library (`libm` / `-lm`)
            - POSIX Threads (`libpthread` / `-lpthread`)
            - Dynamic Linking Loader (`libdl` / `-ldl`)
            - XCB (X C Binding - Windowing & Input):
                - XCB Core (`libxcb`)
                - XCB Image (`libxcb-image`)
                - XCB Sync (`libxcb-sync`)
                - XCB Keysyms (`libxcb-keysyms`)
                - XCB Cursor (`libxcb-cursor`)
            - Graphics:
                - OpenGL (`libGL`)
                - EGL (`libEGL`)

## Building

Compile the build tool:
```sh
clang build.c
```
Building and Running the project:
```sh
./a.out build-run release # For Linux
a.exe build-run release   # For Windows
```
For more build system options/help:
```sh
./a.out --help # For Linux
a.exe --help   # For Windows
```
