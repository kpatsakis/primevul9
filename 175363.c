PyImaging_PcdDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    decoder = PyImaging_DecoderNew(0);
    if (decoder == NULL)
        return NULL;

    /* Unpack from PhotoYCC to RGB */
    if (get_unpacker(decoder, "RGB", "YCC;P") < 0)
        return NULL;

    decoder->decode = ImagingPcdDecode;

    return (PyObject*) decoder;
}