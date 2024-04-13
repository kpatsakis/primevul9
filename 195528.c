new_type_comment(const char *s, struct compiling *c)
{
    PyObject *res = PyUnicode_DecodeUTF8(s, strlen(s), NULL);
    if (res == NULL)
        return NULL;
    if (PyArena_AddPyObject(c->c_arena, res) < 0) {
        Py_DECREF(res);
        return NULL;
    }
    return res;
}