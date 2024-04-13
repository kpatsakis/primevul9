l2f(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, out_ += 4) {
        FLOAT32 f = (FLOAT32)*in++;
        memcpy(out_, &f, sizeof(f));
    }
}