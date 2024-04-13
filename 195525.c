make_kind(struct compiling *c, const node *n)
{
    char *s = NULL;
    PyObject *kind = NULL;

    /* Find the first string literal, if any */
    while (TYPE(n) != STRING) {
        if (NCH(n) == 0)
            return NULL;
        n = CHILD(n, 0);
    }
    REQ(n, STRING);

    /* If it starts with 'u', return a PyUnicode "u" string */
    s = STR(n);
    if (s && *s == 'u') {
        if (!u_kind) {
            u_kind = PyUnicode_InternFromString("u");
            if (!u_kind)
                return NULL;
        }
        kind = u_kind;
        if (PyArena_AddPyObject(c->c_arena, kind) < 0) {
            return NULL;
        }
        Py_INCREF(kind);
    }
    return kind;
}