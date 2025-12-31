# CMono

A C monorepo for my personal projects

## Installation and Setup

### Dependences

- Required
    - C Compiler
        - **GCC**: For Linux
        - **MSVC**: For Windows
    - **Linux only:** XCB libraries (for GUI/windowing)
- Optional
    - **MinGW**: For cross-compiling Windows binaries on Linux
    - **Wine**: For running Windows builds on Linux during development/testing

### Building

- Compile the build tool:
```sh
cc build.c -o build
```
- Build the project:
```sh
./build build     # For Linux
./build.exe build # For Windows
```
- For more build system options/help:
```sh
./build --help     # For Linux
./build.exe --help # For Windows
```

### Running

- Building and Running the program:
```sh
./build build-run      # For Linux
./build.exe build-run  # For Windows
```

## Reference

- [Handmade Hero](https://github.com/cj1128/handmade-hero)
- [OpenGL Xlib Example](https://github.com/vbsw/opengl-xlib-example)
- [Create an OpenGL context using Xlib and EGL](https://gist.github.com/pedrominicz/2d81559c5fb66d23d6bb627570956605)

### Articles/Posts

- [Pre-defined C/C++ Compiler Macros](https://github.com/cpredef/predef)
- [The Arena - Custom Memory Allocators in C](https://www.bytesbeneath.com/p/the-arena-custom-memory-allocators)

### Lib

- [stb](https://github.com/nothings/stb)
- [gb](https://github.com/gingerBill/gb)
- [Handmade Math](https://github.com/HandmadeMath/HandmadeMath)
- [kgflags]( http://github.com/kgabis/kgflags)
- [RGFW](https://github.com/ColleagueRiley/RGFW)
- [nob.h](https://github.com/tsoding/nob.h)

### Projects

- [RAD Debugger](https://github.com/EpicGamesExt/raddebugger)
