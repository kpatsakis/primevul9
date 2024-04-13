make_str_node_and_del(PyObject **str, struct compiling *c, const node* n)
{
    PyObject *s = *str;
    PyObject *kind = NULL;
    *str = NULL;
    assert(PyUnicode_CheckExact(s));
    if (PyArena_AddPyObject(c->c_arena, s) < 0) {
        Py_DECREF(s);
        return NULL;
    }
    kind = make_kind(c, n);
    if (kind == NULL && PyErr_Occurred())
        return NULL;
    return Constant(s, kind, LINENO(n), n->n_col_offset,
                    n->n_end_lineno, n->n_end_col_offset, c->c_arena);
}