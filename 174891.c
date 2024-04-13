ImagingConvert2(Imaging imOut, Imaging imIn) {
    return convert(imOut, imIn, imOut->mode, NULL, 0);
}