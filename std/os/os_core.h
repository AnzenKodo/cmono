//-----------------------------------------------------------------------------
// Reference:
// - By Epic Games Tools & Ryan Fleury: https://github.com/EpicGames/raddebugger/tree/aea4a4ff9d6ed823b432d94b84fe88054f69265b/src/os/core
//-----------------------------------------------------------------------------

#ifndef OS_CORE_H
#define OS_CORE_H

//~ ak: Types
//=============================================================================

typedef struct System_Info System_Info;
struct System_Info
{
    uint32_t logical_processor_count;
    size_t page_size;
    size_t large_page_size;
    uint64_t allocation_granularity;
    size_t physical_memory_size;
    Str8 machine_name;
};

//~ ak: Functions
//=============================================================================

//~ ak: Os Core Functions =====================================================

internal System_Info *os_system_info_get(void);

//- ak: OS Entry Points =======================================================

internal int32_t os_main(void);

#endif // OS_CORE_H
