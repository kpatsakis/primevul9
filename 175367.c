_dealloc(ImagingEncoderObject* encoder)
{
    free(encoder->state.buffer);
    free(encoder->state.context);
    Py_XDECREF(encoder->lock);
    PyObject_Del(encoder);
}