ImagingDestroyArray(Imaging im)
{
    int y;

    if (im->image)
        for (y = 0; y < im->ysize; y++)
            if (im->image[y])
                free(im->image[y]);
}