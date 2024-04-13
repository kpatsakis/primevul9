PyImaging_XbmDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, "1", "1;R") < 0)
        return NULL;

    decoder->decode = ImagingXbmDecode;

    return (PyObject*) decoder;
}