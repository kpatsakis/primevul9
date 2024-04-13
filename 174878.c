I16B_I(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 2, out_ += 4) {
        INT32 v = in[1] + ((int)in[0] << 8);
        memcpy(out_, &v, sizeof(v));
    }
}