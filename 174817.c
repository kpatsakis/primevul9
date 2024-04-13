p2pa(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    for (x = 0; x < xsize; x++, in++) {
        const UINT8 *rgba = &palette[in[0]];
        *out++ = in[0];
        *out++ = in[0];
        *out++ = in[0];
        *out++ = rgba[3];
    }
}