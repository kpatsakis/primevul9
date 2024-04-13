rgb2bgr24(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = in[2];
        *out++ = in[1];
        *out++ = in[0];
    }
}