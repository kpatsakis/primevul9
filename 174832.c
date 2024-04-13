pa2bit(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    /* FIXME: precalculate greyscale palette? */
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = (L(&palette[in[0] * 4]) >= 128000) ? 255 : 0;
    }
}