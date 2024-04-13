ycbcr2la(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4) {
        out[0] = out[1] = out[2] = in[0];
        out[3] = 255;
    }
}