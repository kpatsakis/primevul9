_decode(ImagingDecoderObject* decoder, PyObject* args)
{
    UINT8* buffer;
    int bufsize, status;

    if (!PyArg_ParseTuple(args, PY_ARG_BYTES_LENGTH, &buffer, &bufsize))
        return NULL;

    status = decoder->decode(decoder->im, &decoder->state, buffer, bufsize);

    return Py_BuildValue("ii", status, decoder->state.errcode);
}