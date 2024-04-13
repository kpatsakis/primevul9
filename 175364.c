get_unpacker(ImagingDecoderObject* decoder, const char* mode,
             const char* rawmode)
{
    int bits;
    ImagingShuffler unpack;

    unpack = ImagingFindUnpacker(mode, rawmode, &bits);
    if (!unpack) {
        Py_DECREF(decoder);
        PyErr_SetString(PyExc_ValueError, "unknown raw mode");
        return -1;
    }

    decoder->state.shuffle = unpack;
    decoder->state.bits = bits;

    return 0;
}