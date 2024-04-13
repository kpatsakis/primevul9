pa2ycbcr(UINT8 *out, const UINT8 *in, int xsize, const UINT8 *palette) {
    pa2rgb(out, in, xsize, palette);
    ImagingConvertRGB2YCbCr(out, out, xsize);
}