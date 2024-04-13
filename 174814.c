p2la(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    /* FIXME: precalculate greyscale palette? */
    for (x = 0; x < xsize; x++, out += 4) {
        const UINT8 *rgba = &palette[*in++ * 4];
        out[0] = out[1] = out[2] = L(rgba) / 1000;
        out[3] = rgba[3];
    }
}