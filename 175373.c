PyImaging_RawEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    char *mode;
    char *rawmode;
    int stride = 0;
    int ystep = 1;

    if (!PyArg_ParseTuple(args, "ss|ii", &mode, &rawmode, &stride, &ystep))
	return NULL;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, mode, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingRawEncode;

    encoder->state.ystep = ystep;
    encoder->state.count = stride;

    return (PyObject*) encoder;
}