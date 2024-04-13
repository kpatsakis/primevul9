rgb2bgr15(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out_ += 2) {
        UINT16 v = ((((UINT16)in[0]) << 7) & 0x7c00) +
                   ((((UINT16)in[1]) << 2) & 0x03e0) +
                   ((((UINT16)in[2]) >> 3) & 0x001f);
        memcpy(out_, &v, sizeof(v));
    }
}