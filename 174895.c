p2cmyk(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    p2rgb(out, in, xsize, palette);
    rgb2cmyk(out, out, xsize);
}