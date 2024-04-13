rgb2bgr16(UINT8 *out_, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out_ += 2) {
        UINT16 v = ((((UINT16)in[0]) << 8) & 0xf800) +
                   ((((UINT16)in[1]) << 3) & 0x07e0) +
                   ((((UINT16)in[2]) >> 3) & 0x001f);
        memcpy(out_, &v, sizeof(v));
    }
}