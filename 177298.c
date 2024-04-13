mapping_seek(ImagingMapperObject* mapper, PyObject* args)
{
    int offset;
    int whence = 0;
    if (!PyArg_ParseTuple(args, "i|i", &offset, &whence))
        return NULL;

    switch (whence) {
        case 0: /* SEEK_SET */
            mapper->offset = offset;
            break;
        case 1: /* SEEK_CUR */
            mapper->offset += offset;
            break;
        case 2: /* SEEK_END */
            mapper->offset = mapper->size + offset;
            break;
        default:
            /* FIXME: raise ValueError? */
            break;
    }

    Py_INCREF(Py_None);
    return Py_None;
}