la2rgb(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4) {
        UINT8 v = in[0];
        *out++ = v;
        *out++ = v;
        *out++ = v;
        *out++ = in[3];
    }
}