l2bit(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++) {
        *out++ = (*in++ >= 128) ? 255 : 0;
    }
}