//~ ak: Headers
//=============================================================================

//- ak: headers
#include "std/base/base_include.h"
#include "std/os/os_include.h"

//- ak: implementation
#include "std/base/base_include.c"
#include "std/os/os_include.c"

//~ ak: Example Functions
//=============================================================================

//- ak: 1. Basic unformatted logging
internal int32_t example_basic_logging(void)
{
    Log *log = log_alloc(void)
    log_msg(log, Log_Level_Info,  FILE_PATH, LINE_NUMBER, "this is info log");
    log_msg(log, Log_Level_Debug, FILE_PATH, LINE_NUMBER, "this is debug log");
    log_msg(log, Log_Level_Warn,  FILE_PATH, LINE_NUMBER, "this is warning log");
    log_msg(log, Log_Level_Error, FILE_PATH, LINE_NUMBER, "this is error log");
    
    log_trace_level_set(log_g, Log_Level_Info);
    LogGTraceF("log level is set to Info. this message will not print.");
    LogGInfoF("this is trace Info message");
    LogGDebugF("this is trace Debug message");
    LogGWarnF("this is trace Warn message");
    LogGErrorF("this is trace Error message");
    LogGFatalF("this is trace Fatel message");
    // LogGInfoF("exited on Fatal. this message will not print.");
    
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    for EachEnumVal(Log_Level, level)
    {
        Log_List list = log->list[level];
        for EachNode(node, Log_Node, list.first)
        {
            Fs_File file = OS_STDOUT;
            switch (level)
            {
                case Log_Level_Info: case Log_Level_Debug:
                {
                    file = OS_STDOUT;
                } break;
                case Log_Level_Warn: case Log_Level_Error:
                {
                    file = OS_STDERR;
                } break;
                case Log_Level_COUNT: {} break;
            }
            if (level <= print_level)
            {
                fmt_fprintfln(file, "%s8 %s8:%lu %s8",
                    log_level_string_get(level),
                    node->file_name,
                    node->line_number,
                    node->str);
            }
        }
    }
    arena_scratch_end(scratch);
    
    return 0;
}
