PyImaging_GifEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    char *mode;
    char *rawmode;
    int bits = 8;
    int interlace = 0;
    if (!PyArg_ParseTuple(args, "ss|ii", &mode, &rawmode, &bits, &interlace))
	return NULL;

    encoder = PyImaging_EncoderNew(sizeof(GIFENCODERSTATE));
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, mode, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingGifEncode;

    ((GIFENCODERSTATE*)encoder->state.context)->bits = bits;
    ((GIFENCODERSTATE*)encoder->state.context)->interlace = interlace;

    return (PyObject*) encoder;
}