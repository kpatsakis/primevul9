bit2f(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, out_ += 4) {
        FLOAT32 f = (*in++ != 0) ? 255.0F : 0.0F;
        memcpy(out_, &f, sizeof(f));
    }
}