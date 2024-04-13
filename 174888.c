pa2hsv(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    int x;
    for (x = 0; x < xsize; x++, in += 4, out += 4) {
        const UINT8 *rgb = &palette[in[0] * 4];
        rgb2hsv_row(out, rgb);
        out[3] = 255;
    }
}