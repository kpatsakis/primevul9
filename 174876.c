p2rgb(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    for (x = 0; x < xsize; x++) {
        const UINT8 *rgb = &palette[*in++ * 4];
        *out++ = rgb[0];
        *out++ = rgb[1];
        *out++ = rgb[2];
        *out++ = 255;
    }
}