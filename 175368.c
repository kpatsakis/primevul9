PyImaging_XbmEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    encoder = PyImaging_EncoderNew(0);
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, "1", "1;R") < 0)
	return NULL;

    encoder->encode = ImagingXbmEncode;

    return (PyObject*) encoder;
}