f2l(UINT8 *out, const UINT8 *in_, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, out++, in_ += 4) {
        FLOAT32 v;
        memcpy(&v, in_, sizeof(v));
        if (v <= 0.0) {
            *out = 0;
        } else if (v >= 255.0) {
            *out = 255;
        } else {
            *out = (UINT8)v;
        }
    }
}