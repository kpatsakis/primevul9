PyImaging_ZipEncoderNew(PyObject* self, PyObject* args)
{
    ImagingEncoderObject* encoder;

    char* mode;
    char* rawmode;
    int optimize = 0;
    int compress_level = -1;
    int compress_type = -1;
    char* dictionary = NULL;
    int dictionary_size = 0;
    if (!PyArg_ParseTuple(args, "ss|iii"PY_ARG_BYTES_LENGTH, &mode, &rawmode,
                          &optimize,
                          &compress_level, &compress_type,
                          &dictionary, &dictionary_size))
        return NULL;

    /* Copy to avoid referencing Python's memory, but there's no mechanism to
       free this memory later, so this function (and several others here)
       leaks. */
    if (dictionary && dictionary_size > 0) {
        char* p = malloc(dictionary_size);
        if (!p)
            return PyErr_NoMemory();
        memcpy(p, dictionary, dictionary_size);
        dictionary = p;
    } else
        dictionary = NULL;

    encoder = PyImaging_EncoderNew(sizeof(ZIPSTATE));
    if (encoder == NULL)
	return NULL;

    if (get_packer(encoder, mode, rawmode) < 0)
	return NULL;

    encoder->encode = ImagingZipEncode;

    if (rawmode[0] == 'P')
	/* disable filtering */
	((ZIPSTATE*)encoder->state.context)->mode = ZIP_PNG_PALETTE;

    ((ZIPSTATE*)encoder->state.context)->optimize = optimize;
    ((ZIPSTATE*)encoder->state.context)->compress_level = compress_level;
    ((ZIPSTATE*)encoder->state.context)->compress_type = compress_type;
    ((ZIPSTATE*)encoder->state.context)->dictionary = dictionary;
    ((ZIPSTATE*)encoder->state.context)->dictionary_size = dictionary_size;

    return (PyObject*) encoder;
}