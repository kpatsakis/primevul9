_encode_to_file(ImagingEncoderObject* encoder, PyObject* args)
{
    UINT8* buf;
    int status;
    ImagingSectionCookie cookie;

    /* Encode to a file handle */

    int fh;
    int bufsize = 16384;

    if (!PyArg_ParseTuple(args, "i|i", &fh, &bufsize))
	return NULL;

    /* Allocate an encoder buffer */
    buf = (UINT8*) malloc(bufsize);
    if (!buf)
	return PyErr_NoMemory();

    ImagingSectionEnter(&cookie);

    do {

	/* This replaces the inner loop in the ImageFile _save
	   function. */

	status = encoder->encode(encoder->im, &encoder->state, buf, bufsize);

	if (status > 0)
	    if (write(fh, buf, status) < 0) {
                ImagingSectionLeave(&cookie);
		free(buf);
		return PyErr_SetFromErrno(PyExc_IOError);
	    }

    } while (encoder->state.errcode == 0);

    ImagingSectionLeave(&cookie);

    free(buf);

    return Py_BuildValue("i", encoder->state.errcode);
}