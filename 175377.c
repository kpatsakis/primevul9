PyImaging_GifDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    int bits = 8;
    int interlace = 0;
    if (!PyArg_ParseTuple(args, "s|ii", &mode, &bits, &interlace))
        return NULL;

    if (strcmp(mode, "L") != 0 && strcmp(mode, "P") != 0) {
        PyErr_SetString(PyExc_ValueError, "bad image mode");
        return NULL;
    }

    decoder = PyImaging_DecoderNew(sizeof(GIFDECODERSTATE));
    if (decoder == NULL)
        return NULL;

    decoder->decode = ImagingGifDecode;

    ((GIFDECODERSTATE*)decoder->state.context)->bits = bits;
    ((GIFDECODERSTATE*)decoder->state.context)->interlace = interlace;

    return (PyObject*) decoder;
}