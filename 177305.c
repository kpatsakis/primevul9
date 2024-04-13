mapping_read(ImagingMapperObject* mapper, PyObject* args)
{
    PyObject* buf;

    int size = -1;
    if (!PyArg_ParseTuple(args, "|i", &size))
        return NULL;

    /* check size */
    if (size < 0 || mapper->offset + size > mapper->size)
        size = mapper->size - mapper->offset;
    if (size < 0)
        size = 0;

    buf = PyBytes_FromStringAndSize(NULL, size);
    if (!buf)
        return NULL;

    if (size > 0) {
        memcpy(PyBytes_AsString(buf), mapper->base + mapper->offset, size);
        mapper->offset += size;
    }

    return buf;
}