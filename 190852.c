ImagingDestroyBlock(Imaging im)
{
    if (im->block)
        free(im->block);
}