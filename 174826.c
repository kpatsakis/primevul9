bit2ycbcr(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = (*in++ != 0) ? 255 : 0;
        *out++ = 128;
        *out++ = 128;
        *out++ = 255;
    }
}