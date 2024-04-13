ImagingNew2(const char* mode, Imaging imOut, Imaging imIn)
{
    /* allocate or validate output image */

    if (imOut) {
        /* make sure images match */
        if (strcmp(imOut->mode, mode) != 0
            || imOut->xsize != imIn->xsize
            || imOut->ysize != imIn->ysize) {
            return ImagingError_Mismatch();
        }
    } else {
        /* create new image */
        imOut = ImagingNew(mode, imIn->xsize, imIn->ysize);
        if (!imOut)
            return NULL;
    }

    return imOut;
}