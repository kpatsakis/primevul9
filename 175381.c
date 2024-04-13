PyImaging_TgaRleDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    int ystep = 1;
    int depth = 8;
    if (!PyArg_ParseTuple(args, "ss|ii", &mode, &rawmode, &ystep, &depth))
        return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingTgaRleDecode;

    decoder->state.ystep = ystep;
    decoder->state.count = depth / 8;

    return (PyObject*) decoder;
}