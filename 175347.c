PyImaging_EpsEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    encoder->encode = ImagingEpsEncode;

    return (PyObject*) encoder;
}