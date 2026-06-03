// ak: headers
#include "../base/base_include.h"
#include "../os/os_include.h"

// ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"

internal void print_help_message()
{
    term_style_start(OS_STDOUT, TERM_UNDERLINE);
    fmt_println("USAGE:");
    term_style_end();
    Str8 *program_name = os_program_path_get();
    fmt_printfln("   %s [OPTIONS]", program_name->cstr);
    term_style_start(OS_STDOUT, TERM_UNDERLINE);
    fmt_println("OPTIONS:");
    term_style_end();
    flags_print_help();
    term_style_start(OS_STDOUT, TERM_UNDERLINE);
    fmt_println("VERSION:");
    term_style_end();
    fmt_println("   "APP_VERSION);
}

internal void base_main(void)
{
    FlagsScope()
    {
        Flags_Option *option = NULL;
        bool help = false;
        option = flags_option_bool(str8("help"), &help, help, str8("Print help message"));
        flags_add_option_shortname(option, str8("h"));
        bool version = false;
        option = flags_option_bool(str8("version"), &version, version, str8("Print version message"));
        flags_add_option_shortname(option, str8("v"));
        Str8_Array *args = os_args_get();
        if (!flags_parse(args))
        {
            flags_print_error(flags);
            print_help_message(flags);
            os_exit(1);
        }
        if (help)
        {
            print_help_message(flags);
            os_exit(0);
        }
        if (version)
        {
            fmt_print("v"APP_VERSION);
            os_exit(0);
        }
    }
}
