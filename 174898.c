l2rgb(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++) {
        UINT8 v = *in++;
        *out++ = v;
        *out++ = v;
        *out++ = v;
        *out++ = 255;
    }
}