I16L_F(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 2, out_ += 4) {
        FLOAT32 v = in[0] + ((int)in[1] << 8);
        memcpy(out_, &v, sizeof(v));
    }
}