ImagingConvertInPlace(Imaging imIn, const char *mode) {
    ImagingSectionCookie cookie;
    ImagingShuffler convert;
    int y;

    /* limited support for inplace conversion */
    if (strcmp(imIn->mode, "L") == 0 && strcmp(mode, "1") == 0) {
        convert = l2bit;
    } else if (strcmp(imIn->mode, "1") == 0 && strcmp(mode, "L") == 0) {
        convert = bit2l;
    } else {
        return ImagingError_ModeError();
    }

    ImagingSectionEnter(&cookie);
    for (y = 0; y < imIn->ysize; y++) {
        (*convert)((UINT8 *)imIn->image[y], (UINT8 *)imIn->image[y], imIn->xsize);
    }
    ImagingSectionLeave(&cookie);

    return imIn;
}