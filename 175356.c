PyImaging_DecoderNew(int contextsize)
{
    ImagingDecoderObject *decoder;
    void *context;

    if(PyType_Ready(&ImagingDecoderType) < 0)
        return NULL;

    decoder = PyObject_New(ImagingDecoderObject, &ImagingDecoderType);
    if (decoder == NULL)
        return NULL;

    /* Clear the decoder state */
    memset(&decoder->state, 0, sizeof(decoder->state));

    /* Allocate decoder context */
    if (contextsize > 0) {
        context = (void*) calloc(1, contextsize);
        if (!context) {
            Py_DECREF(decoder);
            (void) PyErr_NoMemory();
            return NULL;
        }
    } else
        context = 0;

    /* Initialize decoder context */
    decoder->state.context = context;

    /* Target image */
    decoder->lock = NULL;
    decoder->im = NULL;

    /* Initialize the cleanup function pointer */
    decoder->cleanup = NULL;

    return decoder;
}