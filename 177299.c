mapping_readimage(ImagingMapperObject* mapper, PyObject* args)
{
    int y, size;
    Imaging im;

    char* mode;
    int xsize;
    int ysize;
    int stride;
    int orientation;
    if (!PyArg_ParseTuple(args, "s(ii)ii", &mode, &xsize, &ysize,
                          &stride, &orientation))
        return NULL;

    if (stride <= 0) {
        /* FIXME: maybe we should call ImagingNewPrologue instead */
        if (!strcmp(mode, "L") || !strcmp(mode, "P"))
            stride = xsize;
        else if (!strcmp(mode, "I;16") || !strcmp(mode, "I;16B"))
            stride = xsize * 2;
        else
            stride = xsize * 4;
    }

    size = ysize * stride;

    if (mapper->offset + size > mapper->size) {
        PyErr_SetString(PyExc_IOError, "image file truncated");
        return NULL;
    }

    im = ImagingNewPrologue(mode, xsize, ysize);
    if (!im)
        return NULL;

    /* setup file pointers */
    if (orientation > 0)
        for (y = 0; y < ysize; y++)
            im->image[y] = mapper->base + mapper->offset + y * stride;
    else
        for (y = 0; y < ysize; y++)
            im->image[ysize-y-1] = mapper->base + mapper->offset + y * stride;

    im->destroy = ImagingDestroyMap;

    if (!ImagingNewEpilogue(im))
        return NULL;

    mapper->offset += size;

    return PyImagingNew(im);
}