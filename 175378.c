PyImaging_JpegDecoderNew(PyObject* self, PyObject* args)
{
    ImagingDecoderObject* decoder;

    char* mode;
    char* rawmode; /* what we wan't from the decoder */
    char* jpegmode; /* what's in the file */
    int scale = 1;
    int draft = 0;
    if (!PyArg_ParseTuple(args, "ssz|ii", &mode, &rawmode, &jpegmode,
                          &scale, &draft))
        return NULL;

    if (!jpegmode)
        jpegmode = "";

    decoder = PyImaging_DecoderNew(sizeof(JPEGSTATE));
    if (decoder == NULL)
        return NULL;

    if (get_unpacker(decoder, mode, rawmode) < 0)
        return NULL;

    decoder->decode = ImagingJpegDecode;
    decoder->cleanup = ImagingJpegDecodeCleanup;

    strncpy(((JPEGSTATE*)decoder->state.context)->rawmode, rawmode, 8);
    strncpy(((JPEGSTATE*)decoder->state.context)->jpegmode, jpegmode, 8);

    ((JPEGSTATE*)decoder->state.context)->scale = scale;
    ((JPEGSTATE*)decoder->state.context)->draft = draft;

    return (PyObject*) decoder;
}