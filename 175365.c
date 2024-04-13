PyImaging_BitDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    int bits  = 8;
    int pad   = 8;
    int fill  = 0;
    int sign  = 0;
    int ystep = 1;
    if (!PyArg_ParseTuple(args, "s|iiiii", &mode, &bits, &pad, &fill,
                          &sign, &ystep))
        return NULL;

    if (strcmp(mode, "F") != 0) {
        PyErr_SetString(PyExc_ValueError, "bad image mode");
        return NULL;
    }

    decoder = PyImaging_DecoderNew(sizeof(BITSTATE));
    if (decoder == NULL)
        return NULL;

    decoder->decode = ImagingBitDecode;

    decoder->state.ystep = ystep;

    ((BITSTATE*)decoder->state.context)->bits = bits;
    ((BITSTATE*)decoder->state.context)->pad  = pad;
    ((BITSTATE*)decoder->state.context)->fill = fill;
    ((BITSTATE*)decoder->state.context)->sign = sign;

    return (PyObject*) decoder;
}