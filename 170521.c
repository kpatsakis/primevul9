static PyObject *ShutdownInterpreter_call(
        ShutdownInterpreterObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *result = NULL;

    result = PyObject_Call(self->wrapped, args, kwds);

    if (result) {
        PyObject *module = NULL;
        PyObject *exitfunc = NULL;

        PyThreadState *tstate = PyThreadState_Get();

        PyThreadState *tstate_save = tstate;
        PyThreadState *tstate_next = NULL;

#if PY_MAJOR_VERSION >= 3
        module = PyImport_ImportModule("atexit");

        if (module) {
            PyObject *dict = NULL;

            dict = PyModule_GetDict(module);
            exitfunc = PyDict_GetItemString(dict, "_run_exitfuncs");
        }
        else
            PyErr_Clear();
#else
        exitfunc = PySys_GetObject("exitfunc");
#endif

        if (exitfunc) {
            PyObject *res = NULL;
            Py_INCREF(exitfunc);
            PySys_SetObject("exitfunc", (PyObject *)NULL);
            res = PyEval_CallObject(exitfunc, (PyObject *)NULL);

            if (res == NULL) {
                PyObject *m = NULL;
                PyObject *result = NULL;

                PyObject *type = NULL;
                PyObject *value = NULL;
                PyObject *traceback = NULL;

                if (PyErr_ExceptionMatches(PyExc_SystemExit)) {
                    Py_BEGIN_ALLOW_THREADS
                    ap_log_error(APLOG_MARK, APLOG_ERR, 0, wsgi_server,
                                 "mod_wsgi (pid=%d): SystemExit exception "
                                 "raised by exit functions ignored.", getpid());
                    Py_END_ALLOW_THREADS
                }
                else {
                    Py_BEGIN_ALLOW_THREADS
                    ap_log_error(APLOG_MARK, APLOG_ERR, 0, wsgi_server,
                                 "mod_wsgi (pid=%d): Exception occurred within "
                                 "exit functions.", getpid());
                    Py_END_ALLOW_THREADS
                }

                PyErr_Fetch(&type, &value, &traceback);
                PyErr_NormalizeException(&type, &value, &traceback);

                if (!value) {
                    value = Py_None;
                    Py_INCREF(value);
                }

                if (!traceback) {
                    traceback = Py_None;
                    Py_INCREF(traceback);
                }

                m = PyImport_ImportModule("traceback");

                if (m) {
                    PyObject *d = NULL;
                    PyObject *o = NULL;
                    d = PyModule_GetDict(m);
                    o = PyDict_GetItemString(d, "print_exception");
                    if (o) {
                        PyObject *log = NULL;
                        PyObject *args = NULL;
                        Py_INCREF(o);
                        log = newLogObject(NULL, APLOG_ERR, NULL);
                        args = Py_BuildValue("(OOOOO)", type, value,
                                             traceback, Py_None, log);
                        result = PyEval_CallObject(o, args);
                        Py_DECREF(args);
                        Py_DECREF(log);
                        Py_DECREF(o);
                    }
                }

                if (!result) {
                    /*
                     * If can't output exception and traceback then
                     * use PyErr_Print to dump out details of the
                     * exception. For SystemExit though if we do
                     * that the process will actually be terminated
                     * so can only clear the exception information
                     * and keep going.
                     */

                    PyErr_Restore(type, value, traceback);

                    if (!PyErr_ExceptionMatches(PyExc_SystemExit)) {
                        PyErr_Print();
                        PyErr_Clear();
                    }
                    else {
                        PyErr_Clear();
                    }
                }
                else {
                    Py_XDECREF(type);
                    Py_XDECREF(value);
                    Py_XDECREF(traceback);
                }

                Py_XDECREF(result);

                Py_XDECREF(m);
            }

            Py_XDECREF(res);
            Py_DECREF(exitfunc);
        }

        Py_XDECREF(module);

        /* Delete remaining thread states. */

        PyThreadState_Swap(NULL);

        tstate = tstate->interp->tstate_head;
        while (tstate) {
            tstate_next = tstate->next;
            if (tstate != tstate_save) {
                PyThreadState_Swap(tstate);
                PyThreadState_Clear(tstate);
                PyThreadState_Swap(NULL);
                PyThreadState_Delete(tstate);
            }
            tstate = tstate_next;
        }
        tstate = tstate_save;

        PyThreadState_Swap(tstate);
    }

    return result;
}