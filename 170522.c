static void Interpreter_dealloc(InterpreterObject *self)
{
    PyThreadState *tstate = NULL;
    PyObject *exitfunc = NULL;
    PyObject *module = NULL;

    PyThreadState *tstate_enter = NULL;

    /*
     * We should always enter here with the Python GIL
     * held and an active thread state. This should only
     * now occur when shutting down interpreter and not
     * when releasing interpreter as don't support
     * recyling of interpreters within the process. Thus
     * the thread state should be that for the main
     * Python interpreter. Where dealing with a named
     * sub interpreter, we need to change the thread
     * state to that which was originally used to create
     * that sub interpreter before doing anything.
     */

    tstate_enter = PyThreadState_Get();

    if (*self->name) {
#if APR_HAS_THREADS
        int thread_id = 0;
        int *thread_handle = NULL;

        apr_threadkey_private_get((void**)&thread_handle, wsgi_thread_key);

        if (!thread_handle) {
            thread_id = wsgi_thread_count++;
            thread_handle = (int*)apr_pmemdup(wsgi_server->process->pool,
                                              &thread_id, sizeof(thread_id));
            apr_threadkey_private_set(thread_handle, wsgi_thread_key);
        }
        else {
            thread_id = *thread_handle;
        }

        tstate = apr_hash_get(self->tstate_table, &thread_id,
                              sizeof(thread_id));

        if (!tstate) {
            tstate = PyThreadState_New(self->interp);

            if (wsgi_server_config->verbose_debugging) {
                ap_log_error(APLOG_MARK, WSGI_LOG_DEBUG(0), wsgi_server,
                             "mod_wsgi (pid=%d): Create thread state for "
                             "thread %d against interpreter '%s'.", getpid(),
                             thread_id, self->name);
            }

            apr_hash_set(self->tstate_table, thread_handle,
                         sizeof(*thread_handle), tstate);
        }
#else
        tstate = self->tstate;
#endif

        /*
         * Swap to interpreter thread state that was used when
         * the sub interpreter was created.
         */

        PyThreadState_Swap(tstate);
    }

    if (self->owner) {
        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Destroy interpreter '%s'.",
                     getpid(), self->name);
        Py_END_ALLOW_THREADS
    }
    else {
        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Cleanup interpreter '%s'.",
                     getpid(), self->name);
        Py_END_ALLOW_THREADS
    }

    /*
     * Because the thread state we are using was created outside
     * of any Python code and is not the same as the Python main
     * thread, there is no record of it within the 'threading'
     * module. We thus need to access current thread function of
     * the 'threading' module to force it to create a thread
     * handle for the thread. If we do not do this, then the
     * 'threading' modules exit function will always fail
     * because it will not be able to find a handle for this
     * thread.
     */

    module = PyImport_ImportModule("threading");

    if (!module)
        PyErr_Clear();

    if (module) {
        PyObject *dict = NULL;
        PyObject *func = NULL;

        dict = PyModule_GetDict(module);
#if PY_MAJOR_VERSION >= 3
        func = PyDict_GetItemString(dict, "current_thread");
#else
        func = PyDict_GetItemString(dict, "currentThread");
#endif
        if (func) {
            PyObject *res = NULL;
            Py_INCREF(func);
            res = PyEval_CallObject(func, (PyObject *)NULL);
            if (!res) {
                PyErr_Clear();
            }
            Py_XDECREF(res);
            Py_DECREF(func);
        }
    }

    /*
     * In Python 2.5.1 an exit function is no longer used to
     * shutdown and wait on non daemon threads which were created
     * from Python code. Instead, in Py_Main() it explicitly
     * calls 'threading._shutdown()'. Thus need to emulate this
     * behaviour for those versions.
     */

#if PY_MAJOR_VERSION < 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4)
    if (module) {
        PyObject *dict = NULL;
        PyObject *func = NULL;

        dict = PyModule_GetDict(module);
        func = PyDict_GetItemString(dict, "_shutdown");
        if (func) {
            PyObject *res = NULL;
            Py_INCREF(func);
            res = PyEval_CallObject(func, (PyObject *)NULL);

            if (res == NULL) {
                PyObject *m = NULL;
                PyObject *result = NULL;

                PyObject *type = NULL;
                PyObject *value = NULL;
                PyObject *traceback = NULL;

                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                             "mod_wsgi (pid=%d): Exception occurred within "
                             "threading._shutdown().", getpid());
                Py_END_ALLOW_THREADS

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
            Py_DECREF(func);
        }
    }

    /* Finally done with 'threading' module. */

    Py_XDECREF(module);

    /*
     * Invoke exit functions by calling sys.exitfunc() for
     * Python 2.X and atexit._run_exitfuncs() for Python 3.X.
     * Note that in Python 3.X we can't call this on main Python
     * interpreter as for Python 3.X it doesn't deregister
     * functions as called, so have no choice but to rely on
     * Py_Finalize() to do it for the main interpreter. Now
     * that simplified GIL state API usage sorted out, this
     * should be okay.
     */

    module = NULL;

#if PY_MAJOR_VERSION >= 3
    if (self->owner) {
        module = PyImport_ImportModule("atexit");

        if (module) {
            PyObject *dict = NULL;

            dict = PyModule_GetDict(module);
            exitfunc = PyDict_GetItemString(dict, "_run_exitfuncs");
        }
        else
            PyErr_Clear();
    }
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
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                             "mod_wsgi (pid=%d): SystemExit exception "
                             "raised by exit functions ignored.", getpid());
                Py_END_ALLOW_THREADS
            }
            else {
                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
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
#endif

    /* If we own it, we destroy it. */

    if (self->owner) {
        /*
         * We need to destroy all the thread state objects
         * associated with the interpreter. If there are
         * background threads that were created then this
         * may well cause them to crash the next time they
         * try to run. Only saving grace is that we are
         * trying to shutdown the process.
         */

#if PY_MAJOR_VERSION < 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 4)
        PyThreadState *tstate_save = tstate;
        PyThreadState *tstate_next = NULL;

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
#endif

        /* Can now destroy the interpreter. */

        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): End interpreter '%s'.",
                     getpid(), self->name);
        Py_END_ALLOW_THREADS

        Py_EndInterpreter(tstate);

        PyThreadState_Swap(tstate_enter);
    }

    free(self->name);

    PyObject_Del(self);
}