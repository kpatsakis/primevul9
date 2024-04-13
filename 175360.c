PyImaging_TiffLzwDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    int filter = 0;
    if (!PyArg_ParseTuple(args, "ss|i", &mode, &rawmode, &filter))
        return NULL;

    decoder = PyImaging_DecoderNew(sizeof(LZWSTATE));
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingLzwDecode;

    ((LZWSTATE*)decoder->state.context)->filter = filter;

    return (PyObject*) decoder;
}