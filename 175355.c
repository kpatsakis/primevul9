PyImaging_RawDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    int stride = 0;
    int ystep  = 1;
    if (!PyArg_ParseTuple(args, "ss|ii", &mode, &rawmode, &stride, &ystep))
        return NULL;

    decoder = PyImaging_DecoderNew(sizeof(RAWSTATE));
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingRawDecode;

    decoder->state.ystep = ystep;

    ((RAWSTATE*)decoder->state.context)->stride = stride;

    return (PyObject*) decoder;
}