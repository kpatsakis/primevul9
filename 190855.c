ImagingDelete(Imaging im)
{
    if (!im)
        return;

    if (im->palette)
        ImagingPaletteDelete(im->palette);

    if (im->destroy)
        im->destroy(im);

    if (im->image)
        free(im->image);

    free(im);
}