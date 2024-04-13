rgb2cmyk(UINT8 *out, const UINT8 *in, int xsize) {
    int x;
    for (x = 0; x < xsize; x++) {
        /* Note: no undercolour removal */
        *out++ = ~(*in++);
        *out++ = ~(*in++);
        *out++ = ~(*in++);
        *out++ = 0;
        in++;
    }
}