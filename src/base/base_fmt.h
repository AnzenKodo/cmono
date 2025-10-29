#ifndef BASE_FMT_H
#define BASE_FMT_H

// External Includes
//=============================================================================

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_DECORATE(name) fmt_##name
#define STB_SPRINTF_STATIC
#include "../external/stb_sprintf.h"

#endif // BASE_FMT_H
