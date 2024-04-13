pa2i(UINT8 *out_, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    INT32 *out = (INT32 *)out_;
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = L(&palette[in[0] * 4]) / 1000;
    }
}