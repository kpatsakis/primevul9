rgb2rgba(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = 255;
        in++;
    }
}