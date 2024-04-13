PyImaging_Mapper(PyObject* self, PyObject* args)
{
    char* filename;
    if (!PyArg_ParseTuple(args, "s", &filename))
        return NULL;

    return (PyObject*) PyImaging_MapperNew(filename, 1);
}