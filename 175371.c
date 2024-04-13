PyImaging_MspDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, "1", "1") < 0)
        return NULL;

    decoder->decode = ImagingMspDecode;

    return (PyObject*) decoder;
}