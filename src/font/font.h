#ifndef FONT_H
#define FONT_H

// External Includes
//=============================================================================

#define STBTT_STATIC
#include "./external/stb_truetype.h"

// Types
//=============================================================================

typedef struct Font Font;
struct Font
{
    stbtt_packedchar data[256];
    unsigned char atlas_width;
    unsigned char atlas_height;
};

#endif // FONT_H
