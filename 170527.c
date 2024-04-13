static void ShutdownInterpreter_dealloc(ShutdownInterpreterObject *self)
{
    Py_DECREF(self->wrapped);
}