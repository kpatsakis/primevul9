pa2f(UINT8 *out_, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    FLOAT32 *out = (FLOAT32 *)out_;
    for (x = 0; x < xsize; x++, in += 4) {
        *out++ = (float)L(&palette[in[0] * 4]) / 1000.0F;
    }
}