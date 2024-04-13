_encode(ImagingEncoderObject* encoder, PyObject* args)
{
    PyObject* buf;
    PyObject* result;
    int status;

    /* Encode to a Python string (allocated by this method) */

    int bufsize = 16384;

    if (!PyArg_ParseTuple(args, "|i", &bufsize))
	return NULL;

    buf = PyBytes_FromStringAndSize(NULL, bufsize);
    if (!buf)
	return NULL;

    status = encoder->encode(encoder->im, &encoder->state,
			     (UINT8*) PyBytes_AsString(buf), bufsize);

    /* adjust string length to avoid slicing in encoder */
    if (_PyBytes_Resize(&buf, (status > 0) ? status : 0) < 0)
        return NULL;

    result = Py_BuildValue("iiO", status, encoder->state.errcode, buf);

    Py_DECREF(buf); /* must release buffer!!! */

    return result;
}