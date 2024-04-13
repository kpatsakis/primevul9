f2i(UINT8 *out_, const UINT8 *in_, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in_ += 4, out_ += 4) {
        FLOAT32 f;
        INT32 i;
        memcpy(&f, in_, sizeof(f));
        i = f;
        memcpy(out_, &i, sizeof(i));
    }
}