mapping_destroy_buffer(Imaging im)
{
    ImagingBufferInstance* buffer = (ImagingBufferInstance*) im;

    PyBuffer_Release(&buffer->view);
    Py_XDECREF(buffer->target);
}