static ShutdownInterpreterObject *newShutdownInterpreterObject(
        PyObject *wrapped)
{
    ShutdownInterpreterObject *self = NULL;

    self = PyObject_New(ShutdownInterpreterObject, &ShutdownInterpreter_Type);
    if (self == NULL)
        return NULL;

    Py_INCREF(wrapped);
    self->wrapped = wrapped;

    return self;
}