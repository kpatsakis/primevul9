la2hsv(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4) {
        UINT8 v = in[0];
        out[0] = 0;
        out[1] = 0;
        out[2] = v;
        out[3] = in[3];
    }
}