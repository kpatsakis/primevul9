L_I16B(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in++) {
        *out++ = 0;
        *out++ = *in;
    }
}