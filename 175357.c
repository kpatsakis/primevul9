PyImaging_ZipDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    int interlaced = 0;
    if (!PyArg_ParseTuple(args, "ss|i", &mode, &rawmode, &interlaced))
        return NULL;

    decoder = PyImaging_DecoderNew(sizeof(ZIPSTATE));
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingZipDecode;

    ((ZIPSTATE*)decoder->state.context)->interlaced = interlaced;

    return (PyObject*) decoder;
}