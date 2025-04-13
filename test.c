void write_stdout(const char *msg, unsigned long len) {
    asm volatile (
        "mov $1, %%rax\n"
        "mov $1, %%rdi\n"
        "mov %0, %%rsi\n"
        "mov %1, %%rdx\n"
        "syscall"
        :
        : "r"(msg), "r"(len)
        : "rax", "rdi", "rsi", "rdx"
    );
}

void my_main() {
    const char msg[] = "Hello\n";
    write_stdout(msg, 6);
}

void _start() {
    my_main();
    asm volatile (
        "mov $60, %rax\n"
        "mov $0, %rdi\n"
        "syscall"
    );
}
