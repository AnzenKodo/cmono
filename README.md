- Void Linux
    - Enable 32bit packages
    ```sh
    sudo xbps-install -S void-repo-multilib && sudo xbps-install -Su
    ```
    - Tools
        - `gcc`: C compiler
        - `gdb`: Debugger
        - `cross-x86_64-w64-mingw32`: Compiler for making Windows exe.
        - `wine`: For running Windows application in linux.
        - `wine-32bit`: For running windows application 32bit version.
    ```sh
    sudo xbps-install gcc gdb cross-x86_64-w64-mingw32 wine wine-32bit
    ```
    - Dep:
    ```sh
    sudo xbps-install libfreeglut-devel xcb-util-image-devel libsanitizer-devel
    ```
