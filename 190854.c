ImagingCopyInfo(Imaging destination, Imaging source)
{
    if (source->palette) {
        if (destination->palette)
            ImagingPaletteDelete(destination->palette);
        destination->palette = ImagingPaletteDuplicate(source->palette);
    }
}