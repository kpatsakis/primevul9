_dealloc(ImagingDecoderObject* decoder)
{
    free(decoder->state.buffer);
    free(decoder->state.context);
    Py_XDECREF(decoder->lock);
    PyObject_Del(decoder);
}