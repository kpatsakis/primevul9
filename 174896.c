ImagingConvert(Imaging imIn, const char *mode, ImagingPalette palette, int dither) {
    return convert(NULL, imIn, mode, palette, dither);
}