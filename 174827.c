L_I16L(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in++) {
        *out++ = *in;
        *out++ = 0;
    }
}