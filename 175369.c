PyImaging_SunRleDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode;
    if (!PyArg_ParseTuple(args, "ss", &mode, &rawmode))
        return NULL;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingSunRleDecode;

    return (PyObject*) decoder;
}