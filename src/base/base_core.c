internal Img img_alloc(Alloc alloc, unsigned int width, unsigned int height, Img_Format format)
{
    Img img = ZERO_STRUCT;
    img.pixels = alloc_make(alloc, uint8_t, width*height);
    img.width = width;
    img.height = height;
    img.format = format;
    return img;
}

internal unsigned int img_pixel_size_from_format(Img_Format format)
{
    unsigned int result = 0;
    switch (format)
    {
        case Img_Format_R8:     { result = 1;  }break;
        case Img_Format_RG8:    { result = 2;  }break;
        case Img_Format_RGBA8:  { result = 4;  }break;
        case Img_Format_BGRA8:  { result = 4;  }break;
        case Img_Format_R16:    { result = 2;  }break;
        case Img_Format_RGBA16: { result = 8;  }break;
        case Img_Format_R32:    { result = 4;  }break;
        case Img_Format_RG32:   { result = 8;  }break;
        case Img_Format_RGBA32: { result = 16; }break;
    }
    return result;
}
