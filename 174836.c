I_I16B(UINT8 *out, const UINT8 *in_, int xsize) {
    int x, v;
    for (x = 0; x < xsize; x++, in_ += 4) {
        INT32 i;
        memcpy(&i, in_, sizeof(i));
        v = CLIP16(i);
        *out++ = (UINT8)(v >> 8);
        *out++ = (UINT8)v;
    }
}