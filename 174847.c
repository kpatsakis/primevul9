rgb2i(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out_ += 4) {
        INT32 v = L24(in) >> 16;
        memcpy(out_, &v, sizeof(v));
    }
}