_setimage(ImagingDecoderObject* decoder, PyObject* args)
{
    PyObject* op;
    Imaging im;
    ImagingCodecState state;
    int x0, y0, x1, y1;

    x0 = y0 = x1 = y1 = 0;

    /* FIXME: should publish the ImagingType descriptor */
    if (!PyArg_ParseTuple(args, "O|(iiii)", &op, &x0, &y0, &x1, &y1))
        return NULL;
    im = PyImaging_AsImaging(op);
    if (!im)
        return NULL;

    decoder->im = im;

    state = &decoder->state;

    /* Setup decoding tile extent */
    if (x0 == 0 && x1 == 0) {
        state->xsize = im->xsize;
        state->ysize = im->ysize;
    } else {
        state->xoff = x0;
        state->yoff = y0;
        state->xsize = x1 - x0;
        state->ysize = y1 - y0;
    }

    if (state->xsize <= 0 ||
        state->xsize + state->xoff > (int) im->xsize ||
        state->ysize <= 0 ||
        state->ysize + state->yoff > (int) im->ysize) {
        PyErr_SetString(PyExc_ValueError, "tile cannot extend outside image");
        return NULL;
    }

    /* Allocate memory buffer (if bits field is set) */
    if (state->bits > 0) {
        if (!state->bytes)
            state->bytes = (state->bits * state->xsize+7)/8;
        state->buffer = (UINT8*) malloc(state->bytes);
        if (!state->buffer)
            return PyErr_NoMemory();
    }

    /* Keep a reference to the image object, to make sure it doesn't
       go away before we do */
    Py_INCREF(op);
    Py_XDECREF(decoder->lock);
    decoder->lock = op;

    Py_INCREF(Py_None);
    return Py_None;
}