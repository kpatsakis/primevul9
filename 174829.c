lA2la(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    unsigned int alpha, pixel, tmp;
    for (x = 0; x < xsize; x++, in += 4) {
        alpha = in[3];
        pixel = MULDIV255(in[0], alpha, tmp);
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)pixel;
        *out++ = (UINT8)alpha;
    }
}