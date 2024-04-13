p2rgba(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    for (x = 0; x < xsize; x++) {
        const UINT8 *rgba = &palette[*in++ * 4];
        *out++ = rgba[0];
        *out++ = rgba[1];
        *out++ = rgba[2];
        *out++ = rgba[3];
    }
}