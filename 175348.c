PyImaging_PcxDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    int stride;
    if (!PyArg_ParseTuple(args, "ssi", &mode, &rawmode, &stride))
        return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->state.bytes = stride;

    decoder->decode = ImagingPcxDecode;

    return (PyObject*) decoder;
}