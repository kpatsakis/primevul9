PyImaging_JpegEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    char *mode;
    char *rawmode;
    int quality = 0;
    int progressive = 0;
    int smooth = 0;
    int optimize = 0;
    int streamtype = 0; /* 0=interchange, 1=tables only, 2=image only */
    int xdpi = 0, ydpi = 0;
    int subsampling = -1; /* -1=default, 0=none, 1=medium, 2=high */
    PyObject* qtables=NULL;
    unsigned int **qarrays = NULL;
    char* extra = NULL;
    int extra_size;
    char* rawExif = NULL;
    int rawExifLen = 0;

    if (!PyArg_ParseTuple(args, "ss|iiiiiiiiO"PY_ARG_BYTES_LENGTH""PY_ARG_BYTES_LENGTH,
                          &mode, &rawmode, &quality,
                          &progressive, &smooth, &optimize, &streamtype,
                          &xdpi, &ydpi, &subsampling, &qtables, &extra, &extra_size,
                          &rawExif, &rawExifLen))
	return NULL;

    encoder = PyImaging_EncoderNew(sizeof(JPEGENCODERSTATE));
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, mode, rawmode) < 0)
	return NULL;

    qarrays = get_qtables_arrays(qtables);

    if (extra && extra_size > 0) {
        char* p = malloc(extra_size);
        if (!p)
            return PyErr_NoMemory();
        memcpy(p, extra, extra_size);
        extra = p;
    } else
        extra = NULL;

    if (rawExif && rawExifLen > 0) {
        char* pp = malloc(rawExifLen);
        if (!pp)
            return PyErr_NoMemory();
        memcpy(pp, rawExif, rawExifLen);
        rawExif = pp;
    } else
        rawExif = NULL;

    encoder->encode = ImagingJpegEncode;

    ((JPEGENCODERSTATE*)encoder->state.context)->quality = quality;
    ((JPEGENCODERSTATE*)encoder->state.context)->qtables = qarrays;
    ((JPEGENCODERSTATE*)encoder->state.context)->subsampling = subsampling;
    ((JPEGENCODERSTATE*)encoder->state.context)->progressive = progressive;
    ((JPEGENCODERSTATE*)encoder->state.context)->smooth = smooth;
    ((JPEGENCODERSTATE*)encoder->state.context)->optimize = optimize;
    ((JPEGENCODERSTATE*)encoder->state.context)->streamtype = streamtype;
    ((JPEGENCODERSTATE*)encoder->state.context)->xdpi = xdpi;
    ((JPEGENCODERSTATE*)encoder->state.context)->ydpi = ydpi;
    ((JPEGENCODERSTATE*)encoder->state.context)->extra = extra;
    ((JPEGENCODERSTATE*)encoder->state.context)->extra_size = extra_size;
    ((JPEGENCODERSTATE*)encoder->state.context)->rawExif = rawExif;
    ((JPEGENCODERSTATE*)encoder->state.context)->rawExifLen = rawExifLen;

    return (PyObject*) encoder;
}