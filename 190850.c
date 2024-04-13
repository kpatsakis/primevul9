ImagingNewPrologueSubtype(const char *mode, int xsize, int ysize,
                          int size)
{
    Imaging im;
    ImagingSectionCookie cookie;

    im = (Imaging) calloc(1, size);
    if (!im)
        return (Imaging) ImagingError_MemoryError();

    /* linesize overflow check, roughly the current largest space req'd */
    if (xsize > (INT_MAX / 4) - 1) {
        return (Imaging) ImagingError_MemoryError();
    }

    /* Setup image descriptor */
    im->xsize = xsize;
    im->ysize = ysize;

    im->type = IMAGING_TYPE_UINT8;

    if (strcmp(mode, "1") == 0) {
        /* 1-bit images */
        im->bands = im->pixelsize = 1;
        im->linesize = xsize;

    } else if (strcmp(mode, "P") == 0) {
        /* 8-bit palette mapped images */
        im->bands = im->pixelsize = 1;
        im->linesize = xsize;
        im->palette = ImagingPaletteNew("RGB");

    } else if (strcmp(mode, "PA") == 0) {
        /* 8-bit palette with alpha */
        im->bands = 2;
        im->pixelsize = 4; /* store in image32 memory */
        im->linesize = xsize * 4;
        im->palette = ImagingPaletteNew("RGB");

    } else if (strcmp(mode, "L") == 0) {
        /* 8-bit greyscale (luminance) images */
        im->bands = im->pixelsize = 1;
        im->linesize = xsize;

    } else if (strcmp(mode, "LA") == 0) {
        /* 8-bit greyscale (luminance) with alpha */
        im->bands = 2;
        im->pixelsize = 4; /* store in image32 memory */
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "La") == 0) {
        /* 8-bit greyscale (luminance) with premultiplied alpha */
        im->bands = 2;
        im->pixelsize = 4; /* store in image32 memory */
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "F") == 0) {
        /* 32-bit floating point images */
        im->bands = 1;
        im->pixelsize = 4;
        im->linesize = xsize * 4;
        im->type = IMAGING_TYPE_FLOAT32;

    } else if (strcmp(mode, "I") == 0) {
        /* 32-bit integer images */
        im->bands = 1;
        im->pixelsize = 4;
        im->linesize = xsize * 4;
        im->type = IMAGING_TYPE_INT32;

    } else if (strcmp(mode, "I;16") == 0 || strcmp(mode, "I;16L") == 0 \
                           || strcmp(mode, "I;16B") == 0 || strcmp(mode, "I;16N") == 0)  {
        /* EXPERIMENTAL */
        /* 16-bit raw integer images */
        im->bands = 1;
        im->pixelsize = 2;
        im->linesize = xsize * 2;
        im->type = IMAGING_TYPE_SPECIAL;

    } else if (strcmp(mode, "RGB") == 0) {
        /* 24-bit true colour images */
        im->bands = 3;
        im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "BGR;15") == 0) {
        /* EXPERIMENTAL */
        /* 15-bit true colour */
        im->bands = 1;
        im->pixelsize = 2;
        im->linesize = (xsize*2 + 3) & -4;
        im->type = IMAGING_TYPE_SPECIAL;

    } else if (strcmp(mode, "BGR;16") == 0) {
        /* EXPERIMENTAL */
        /* 16-bit reversed true colour */
        im->bands = 1;
        im->pixelsize = 2;
        im->linesize = (xsize*2 + 3) & -4;
        im->type = IMAGING_TYPE_SPECIAL;

    } else if (strcmp(mode, "BGR;24") == 0) {
        /* EXPERIMENTAL */
        /* 24-bit reversed true colour */
        im->bands = 1;
        im->pixelsize = 3;
        im->linesize = (xsize*3 + 3) & -4;
        im->type = IMAGING_TYPE_SPECIAL;

    } else if (strcmp(mode, "BGR;32") == 0) {
        /* EXPERIMENTAL */
        /* 32-bit reversed true colour */
        im->bands = 1;
        im->pixelsize = 4;
        im->linesize = (xsize*4 + 3) & -4;
        im->type = IMAGING_TYPE_SPECIAL;

    } else if (strcmp(mode, "RGBX") == 0) {
        /* 32-bit true colour images with padding */
        im->bands = im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "RGBA") == 0) {
        /* 32-bit true colour images with alpha */
        im->bands = im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "RGBa") == 0) {
        /* EXPERIMENTAL */
        /* 32-bit true colour images with premultiplied alpha */
        im->bands = im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "CMYK") == 0) {
        /* 32-bit colour separation */
        im->bands = im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "YCbCr") == 0) {
        /* 24-bit video format */
        im->bands = 3;
        im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "LAB") == 0) {
        /* 24-bit color, luminance, + 2 color channels */
        /* L is uint8, a,b are int8 */
        im->bands = 3;
        im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else if (strcmp(mode, "HSV") == 0) {
        /* 24-bit color, luminance, + 2 color channels */
        /* L is uint8, a,b are int8 */
        im->bands = 3;
        im->pixelsize = 4;
        im->linesize = xsize * 4;

    } else {
        free(im);
        return (Imaging) ImagingError_ValueError("unrecognized mode");
    }

    /* Setup image descriptor */
    strcpy(im->mode, mode);

    ImagingSectionEnter(&cookie);

    /* Pointer array (allocate at least one line, to avoid MemoryError
       exceptions on platforms where calloc(0, x) returns NULL) */
    im->image = (char **) calloc((ysize > 0) ? ysize : 1, sizeof(void *));

    ImagingSectionLeave(&cookie);

    if (!im->image) {
        free(im);
        return (Imaging) ImagingError_MemoryError();
    }

    ImagingNewCount++;

    return im;
}