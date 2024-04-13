rgb2f(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out_ += 4) {
        FLOAT32 v = (float)L(in) / 1000.0F;
        memcpy(out_, &v, sizeof(v));
    }
}