ImagingNewBlock(const char *mode, int xsize, int ysize)
{
    Imaging im;
    Py_ssize_t y, i;

    im = ImagingNewPrologue(mode, xsize, ysize);
    if (!im)
        return NULL;

    /* We shouldn't overflow, since the threshold defined
       below says that we're only going to allocate max 4M
       here before going to the array allocator. Check anyway.
    */
    if (im->linesize &&
        im->ysize > INT_MAX / im->linesize) {
        /* punt if we're going to overflow */
        return NULL;
    }

    if (im->ysize * im->linesize <= 0) {
        /* some platforms return NULL for malloc(0); this fix
           prevents MemoryError on zero-sized images on such
           platforms */
        im->block = (char *) malloc(1);
    } else {
        /* malloc check ok, overflow check above */
        im->block = (char *) calloc(im->ysize, im->linesize);
    }

    if (im->block) {
        for (y = i = 0; y < im->ysize; y++) {
            im->image[y] = im->block + i;
            i += im->linesize;
        }

        im->destroy = ImagingDestroyBlock;

    }

    return ImagingNewEpilogue(im);
}