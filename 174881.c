p2i(UINT8 *out_, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    for (x = 0; x < xsize; x++, out_ += 4) {
        INT32 v = L(&palette[in[x] * 4]) / 1000;
        memcpy(out_, &v, sizeof(v));
    }
}