pa2cmyk(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    pa2rgb(out, in, xsize, palette);
    rgb2cmyk(out, out, xsize);
}