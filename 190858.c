ImagingNewEpilogue(Imaging im)
{
    /* If the raster data allocator didn't setup a destructor,
       assume that it couldn't allocate the required amount of
       memory. */
    if (!im->destroy)
        return (Imaging) ImagingError_MemoryError();

    /* Initialize alias pointers to pixel data. */
    switch (im->pixelsize) {
    case 1: case 2: case 3:
        im->image8 = (UINT8 **) im->image;
        break;
    case 4:
        im->image32 = (INT32 **) im->image;
        break;
    }

    return im;
}