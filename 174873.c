l2i(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, out_ += 4) {
        INT32 v = *in++;
        memcpy(out_, &v, sizeof(v));
    }
}