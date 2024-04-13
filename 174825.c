I16B_L(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 2) {
        if (in[0] != 0) {
            *out++ = 255;
        } else {
            *out++ = in[1];
        }
    }
}