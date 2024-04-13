frompalette(Imaging imOut, Imaging imIn, const char *mode) {
    ImagingSectionCookie cookie;
    int alpha;
    int y;
    void (*convert)(UINT8 *, const UINT8 *, int, const UINT8 *);

    /* Map palette image to L, RGB, RGBA, or CMYK */

    if (!imIn->palette) {
        return (Imaging)ImagingError_ValueError("no palette");
    }

    alpha = !strcmp(imIn->mode, "PA");

    if (strcmp(mode, "1") == 0) {
        convert = alpha ? pa2bit : p2bit;
    } else if (strcmp(mode, "L") == 0) {
        convert = alpha ? pa2l : p2l;
    } else if (strcmp(mode, "LA") == 0) {
        convert = alpha ? pa2la : p2la;
    } else if (strcmp(mode, "PA") == 0) {
        convert = p2pa;
    } else if (strcmp(mode, "I") == 0) {
        convert = alpha ? pa2i : p2i;
    } else if (strcmp(mode, "F") == 0) {
        convert = alpha ? pa2f : p2f;
    } else if (strcmp(mode, "RGB") == 0) {
        convert = alpha ? pa2rgb : p2rgb;
    } else if (strcmp(mode, "RGBA") == 0) {
        convert = alpha ? pa2rgba : p2rgba;
    } else if (strcmp(mode, "RGBX") == 0) {
        convert = alpha ? pa2rgba : p2rgba;
    } else if (strcmp(mode, "CMYK") == 0) {
        convert = alpha ? pa2cmyk : p2cmyk;
    } else if (strcmp(mode, "YCbCr") == 0) {
        convert = alpha ? pa2ycbcr : p2ycbcr;
    } else if (strcmp(mode, "HSV") == 0) {
        convert = alpha ? pa2hsv : p2hsv;
    } else {
        return (Imaging)ImagingError_ValueError("conversion not supported");
    }

    imOut = ImagingNew2Dirty(mode, imOut, imIn);
    if (!imOut) {
        return NULL;
    }

    ImagingSectionEnter(&cookie);
    for (y = 0; y < imIn->ysize; y++) {
        (*convert)(
            (UINT8 *)imOut->image[y],
            (UINT8 *)imIn->image[y],
            imIn->xsize,
            imIn->palette->palette);
    }
    ImagingSectionLeave(&cookie);

    return imOut;
}