i2hsv(UINT8 *out, const UINT8 *in_, int xsize) {
    int x;
    INT32 *in = (INT32 *)in_;
    for (x = 0; x < xsize; x++, in++, out += 4) {
        out[0] = 0;
        out[1] = 0;
        if (*in <= 0) {
            out[2] = 0;
        } else if (*in >= 255) {
            out[2] = 255;
        } else {
            out[2] = (UINT8)*in;
        }
        out[3] = 255;
    }
}