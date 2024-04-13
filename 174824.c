tobilevel(Imaging imOut, Imaging imIn, int dither) {
    ImagingSectionCookie cookie;
    int x, y;
    int *errors;

    /* Map L or RGB to dithered 1 image */
    if (strcmp(imIn->mode, "L") != 0 && strcmp(imIn->mode, "RGB") != 0) {
        return (Imaging)ImagingError_ValueError("conversion not supported");
    }

    imOut = ImagingNew2Dirty("1", imOut, imIn);
    if (!imOut) {
        return NULL;
    }

    errors = calloc(imIn->xsize + 1, sizeof(int));
    if (!errors) {
        ImagingDelete(imOut);
        return ImagingError_MemoryError();
    }

    if (imIn->bands == 1) {
        /* map each pixel to black or white, using error diffusion */
        ImagingSectionEnter(&cookie);
        for (y = 0; y < imIn->ysize; y++) {
            int l, l0, l1, l2, d2;
            UINT8 *in = (UINT8 *)imIn->image[y];
            UINT8 *out = imOut->image8[y];

            l = l0 = l1 = 0;

            for (x = 0; x < imIn->xsize; x++) {
                /* pick closest colour */
                l = CLIP8(in[x] + (l + errors[x + 1]) / 16);
                out[x] = (l > 128) ? 255 : 0;

                /* propagate errors */
                l -= (int)out[x];
                l2 = l;
                d2 = l + l;
                l += d2;
                errors[x] = l + l0;
                l += d2;
                l0 = l + l1;
                l1 = l2;
                l += d2;
            }

            errors[x] = l0;
        }
        ImagingSectionLeave(&cookie);

    } else {
        /* map each pixel to black or white, using error diffusion */
        ImagingSectionEnter(&cookie);
        for (y = 0; y < imIn->ysize; y++) {
            int l, l0, l1, l2, d2;
            UINT8 *in = (UINT8 *)imIn->image[y];
            UINT8 *out = imOut->image8[y];

            l = l0 = l1 = 0;

            for (x = 0; x < imIn->xsize; x++, in += 4) {
                /* pick closest colour */
                l = CLIP8(L(in) / 1000 + (l + errors[x + 1]) / 16);
                out[x] = (l > 128) ? 255 : 0;

                /* propagate errors */
                l -= (int)out[x];
                l2 = l;
                d2 = l + l;
                l += d2;
                errors[x] = l + l0;
                l += d2;
                l0 = l + l1;
                l1 = l2;
                l += d2;
            }

            errors[x] = l0;
        }
        ImagingSectionLeave(&cookie);
    }

    free(errors);

    return imOut;
}