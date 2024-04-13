PyImaging_EncoderNew(int contextsize)
{
    ImagingEncoderObject *encoder;
    void *context;

    if(!PyType_Ready(&ImagingEncoderType) < 0)
        return NULL;

    encoder = PyObject_New(ImagingEncoderObject, &ImagingEncoderType);
    if (encoder == NULL)
	return NULL;

    /* Clear the encoder state */
    memset(&encoder->state, 0, sizeof(encoder->state));

    /* Allocate encoder context */
    if (contextsize > 0) {
	context = (void*) calloc(1, contextsize);
	if (!context) {
	    Py_DECREF(encoder);
	    (void) PyErr_NoMemory();
	    return NULL;
	}
    } else
	context = 0;

    /* Initialize encoder context */
    encoder->state.context = context;

    /* Target image */
    encoder->lock = NULL;
    encoder->im = NULL;

    return encoder;
}