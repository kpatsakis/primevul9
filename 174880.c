la2cmyk(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = 0;
        *out++ = 0;
        *out++ = 0;
        *out++ = ~(in[0]);
    }
}