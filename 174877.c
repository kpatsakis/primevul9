i2f(UINT8 *out_, const UINT8 *in_, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in_ += 4, out_ += 4) {
        INT32 i;
        FLOAT32 f;
        memcpy(&i, in_, sizeof(i));
        f = i;
        memcpy(out_, &f, sizeof(f));
    }
}