l2hsv(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, out += 4) {
        UINT8 v = *in++;
        out[0] = 0;
        out[1] = 0;
        out[2] = v;
        out[3] = 255;
    }
}