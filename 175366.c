PyImaging_FliDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    decoder->decode = ImagingFliDecode;

    return (PyObject*) decoder;
}