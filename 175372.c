PyImaging_PcxEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    char *mode;
    char *rawmode;
    int bits = 8;
    if (!PyArg_ParseTuple(args, "ss|ii", &mode, &rawmode, &bits))
	return NULL;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, mode, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingPcxEncode;

    return (PyObject*) encoder;
}