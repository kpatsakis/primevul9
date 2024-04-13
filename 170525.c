static InterpreterObject *newInterpreterObject(const char *name)
{
    PyInterpreterState *interp = NULL;
    InterpreterObject *self = NULL;
    PyThreadState *tstate = NULL;
    PyThreadState *save_tstate = NULL;
    PyObject *module = NULL;
    PyObject *object = NULL;
    PyObject *item = NULL;

    /* Create handle for interpreter and local data. */

    self = PyObject_New(InterpreterObject, &Interpreter_Type);
    if (self == NULL)
        return NULL;

    /*
     * If interpreter not named, then we want to bind
     * to the first Python interpreter instance created.
     * Give this interpreter an empty string as name.
     */

    if (!name) {
        interp = PyInterpreterState_Head();
        while (interp->next)
            interp = interp->next;

        name = "";
    }

    /* Save away the interpreter name. */

    self->name = strdup(name);

    if (interp) {
        /*
         * Interpreter provided to us so will not be
         * responsible for deleting it later. This will
         * be the case for the main Python interpreter.
         */

        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Attach interpreter '%s'.",
                     getpid(), name);

        self->interp = interp;
        self->owner = 0;
    }
    else {
        PyObject *module = NULL;

        /*
         * Remember active thread state so can restore
         * it. This is actually the thread state
         * associated with simplified GIL state API.
         */

        save_tstate = PyThreadState_Swap(NULL);

        /*
         * Create the interpreter. If creation of the
         * interpreter fails it will restore the
         * existing active thread state for us so don't
         * need to worry about it in that case.
         */

        tstate = Py_NewInterpreter();

        if (!tstate) {
            PyErr_SetString(PyExc_RuntimeError, "Py_NewInterpreter() failed");

            Py_DECREF(self);

            return NULL;
        }

        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Create interpreter '%s'.",
                     getpid(), name);
        Py_END_ALLOW_THREADS

        self->interp = tstate->interp;
        self->owner = 1;

        /*
         * We need to replace threading._shutdown() with our own
         * function which will also call atexit callbacks after
         * threads are shutdown to cope with fact that Python
         * itself doesn't call the atexit callbacks in sub
         * interpreters.
         */

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
        module = PyImport_ImportModule("threading");

        if (module) {
            PyObject *dict = NULL;
            PyObject *func = NULL;

            dict = PyModule_GetDict(module);
            func = PyDict_GetItemString(dict, "_shutdown");

            if (func) {
                PyObject *wrapper = NULL;

                wrapper = newShutdownInterpreterObject(func);
                PyDict_SetItemString(dict, "_shutdown", wrapper);
                Py_DECREF(wrapper);
            }
        }

        Py_XDECREF(module);
#endif
    }

    /*
     * Install restricted objects for STDIN and STDOUT,
     * or log object for STDOUT as appropriate. Don't do
     * this if not running on Win32 and we believe we
     * are running in single process mode, otherwise
     * it prevents use of interactive debuggers such as
     * the 'pdb' module.
     */

    object = newLogObject(NULL, APLOG_ERR, "stderr");
    PySys_SetObject("stderr", object);
    Py_DECREF(object);

#ifndef WIN32
    if (wsgi_parent_pid != getpid()) {
#endif
        if (wsgi_server_config->restrict_stdout == 1) {
            object = (PyObject *)newRestrictedObject("sys.stdout");
            PySys_SetObject("stdout", object);
            Py_DECREF(object);
        }
        else {
            object = newLogObject(NULL, APLOG_ERR, "stdout");
            PySys_SetObject("stdout", object);
            Py_DECREF(object);
        }

        if (wsgi_server_config->restrict_stdin == 1) {
            object = (PyObject *)newRestrictedObject("sys.stdin");
            PySys_SetObject("stdin", object);
            Py_DECREF(object);
        }
#ifndef WIN32
    }
#endif

    /*
     * Set sys.argv to one element list to fake out
     * modules that look there for Python command
     * line arguments as appropriate.
     */

    object = PyList_New(0);
#if PY_MAJOR_VERSION >= 3
    item = PyUnicode_FromString("mod_wsgi");
#else
    item = PyString_FromString("mod_wsgi");
#endif
    PyList_Append(object, item);
    PySys_SetObject("argv", object);
    Py_DECREF(item);
    Py_DECREF(object);

    /*
     * Install intercept for signal handler registration
     * if appropriate.
     */

    if (wsgi_server_config->restrict_signal != 0) {
        module = PyImport_ImportModule("signal");
        PyModule_AddObject(module, "signal", PyCFunction_New(
                           &wsgi_signal_method[0], NULL));
        Py_DECREF(module);
    }

    /*
     * Force loading of codecs into interpreter. This has to be
     * done as not otherwise done in sub interpreters and if not
     * done, code running in sub interpreters can fail on some
     * platforms if a unicode string is added in sys.path and an
     * import then done.
     */

    item = PyCodec_Encoder("ascii");
    Py_XDECREF(item);

    /*
     * If running in daemon process, override as appropriate
     * the USER, USERNAME or LOGNAME environment  variables
     * so that they match the user that the process is running
     * as. Need to do this else we inherit the value from the
     * Apache parent process which is likely wrong as will be
     * root or the user than ran sudo when Apache started.
     * Can't update these for normal Apache child processes
     * as that would change the expected environment of other
     * Apache modules.
     */

#ifndef WIN32
    if (wsgi_daemon_pool) {
        module = PyImport_ImportModule("os");

        if (module) {
            PyObject *dict = NULL;
            PyObject *key = NULL;
            PyObject *value = NULL;

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "environ");

            if (object) {
                struct passwd *pwent;

                pwent = getpwuid(geteuid());

                if (getenv("USER")) {
#if PY_MAJOR_VERSION >= 3
                    key = PyUnicode_FromString("USER");
                    value = PyUnicode_Decode(pwent->pw_name,
                                             strlen(pwent->pw_name),
                                             Py_FileSystemDefaultEncoding,
                                             "surrogateescape");
#else
                    key = PyString_FromString("USER");
                    value = PyString_FromString(pwent->pw_name);
#endif

                    PyObject_SetItem(object, key, value);

                    Py_DECREF(key);
                    Py_DECREF(value);
                }

                if (getenv("USERNAME")) {
#if PY_MAJOR_VERSION >= 3
                    key = PyUnicode_FromString("USERNAME");
                    value = PyUnicode_Decode(pwent->pw_name,
                                             strlen(pwent->pw_name),
                                             Py_FileSystemDefaultEncoding,
                                             "surrogateescape");
#else
                    key = PyString_FromString("USERNAME");
                    value = PyString_FromString(pwent->pw_name);
#endif

                    PyObject_SetItem(object, key, value);

                    Py_DECREF(key);
                    Py_DECREF(value);
                }

                if (getenv("LOGNAME")) {
#if PY_MAJOR_VERSION >= 3
                    key = PyUnicode_FromString("LOGNAME");
                    value = PyUnicode_Decode(pwent->pw_name,
                                             strlen(pwent->pw_name),
                                             Py_FileSystemDefaultEncoding,
                                             "surrogateescape");
#else
                    key = PyString_FromString("LOGNAME");
                    value = PyString_FromString(pwent->pw_name);
#endif

                    PyObject_SetItem(object, key, value);

                    Py_DECREF(key);
                    Py_DECREF(value);
                }
            }

            Py_DECREF(module);
        }
    }
#endif

    /*
     * If running in daemon process, override HOME environment
     * variable so that is matches the home directory of the
     * user that the process is running as. Need to do this as
     * Apache will inherit HOME from root user or user that ran
     * sudo and started Apache and this would be wrong. Can't
     * update HOME for normal Apache child processes as that
     * would change the expected environment of other Apache
     * modules.
     */

#ifndef WIN32
    if (wsgi_daemon_pool) {
        module = PyImport_ImportModule("os");

        if (module) {
            PyObject *dict = NULL;
            PyObject *key = NULL;
            PyObject *value = NULL;

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "environ");

            if (object) {
                struct passwd *pwent;

                pwent = getpwuid(geteuid());
#if PY_MAJOR_VERSION >= 3
                key = PyUnicode_FromString("HOME");
                value = PyUnicode_Decode(pwent->pw_dir, strlen(pwent->pw_dir),
                                         Py_FileSystemDefaultEncoding,
                                         "surrogateescape");
#else
                key = PyString_FromString("HOME");
                value = PyString_FromString(pwent->pw_dir);
#endif

                PyObject_SetItem(object, key, value);

                Py_DECREF(key);
                Py_DECREF(value);
            }

            Py_DECREF(module);
        }
    }
#endif

    /*
     * Explicitly override the PYTHON_EGG_CACHE variable if it
     * was defined by Apache configuration. For embedded processes
     * this would have been done by using WSGIPythonEggs directive.
     * For daemon processes the 'python-eggs' option to the
     * WSGIDaemonProcess directive would have needed to be used.
     */

    if (!wsgi_daemon_pool)
        wsgi_python_eggs = wsgi_server_config->python_eggs;

    if (wsgi_python_eggs) {
        module = PyImport_ImportModule("os");

        if (module) {
            PyObject *dict = NULL;
            PyObject *key = NULL;
            PyObject *value = NULL;

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "environ");

            if (object) {
#if PY_MAJOR_VERSION >= 3
                key = PyUnicode_FromString("PYTHON_EGG_CACHE");
                value = PyUnicode_Decode(wsgi_python_eggs,
                                         strlen(wsgi_python_eggs),
                                         Py_FileSystemDefaultEncoding,
                                         "surrogateescape");
#else
                key = PyString_FromString("PYTHON_EGG_CACHE");
                value = PyString_FromString(wsgi_python_eggs);
#endif

                PyObject_SetItem(object, key, value);

                Py_DECREF(key);
                Py_DECREF(value);
            }

            Py_DECREF(module);
        }
    }

    /*
     * Install user defined Python module search path. This is
     * added using site.addsitedir() so that any Python .pth
     * files are opened and additional directories so defined
     * are added to default Python search path as well. This
     * allows virtual Python environments to work. Note that
     * site.addsitedir() adds new directories at the end of
     * sys.path when they really need to be added in order at
     * the start. We therefore need to do a fiddle and shift
     * any newly added directories to the start of sys.path.
     */

    if (!wsgi_daemon_pool)
        wsgi_python_path = wsgi_server_config->python_path;

    if (wsgi_python_path) {
        PyObject *path = NULL;

        module = PyImport_ImportModule("site");
        path = PySys_GetObject("path");

        if (module && path) {
            PyObject *dict = NULL;

            PyObject *old = NULL;
            PyObject *new = NULL;
            PyObject *tmp = NULL;

            PyObject *item = NULL;

            int i = 0;

            old = PyList_New(0);
            new = PyList_New(0);
            tmp = PyList_New(0);

            for (i=0; i<PyList_Size(path); i++)
                PyList_Append(old, PyList_GetItem(path, i));

            dict = PyModule_GetDict(module);
            object = PyDict_GetItemString(dict, "addsitedir");

            if (object) {
                const char *start;
                const char *end;
                const char *value;

                PyObject *item;
                PyObject *args;

                PyObject *result = NULL;

                Py_INCREF(object);

                start = wsgi_python_path;
                end = strchr(start, DELIM);

                if (end) {
#if PY_MAJOR_VERSION >= 3
                    item = PyUnicode_Decode(start, end-start,
                                            Py_FileSystemDefaultEncoding,
                                            "surrogateescape");
#else
                    item = PyString_FromStringAndSize(start, end-start);
#endif
                    start = end+1;

                    value = PyString_AsString(item);

                    Py_BEGIN_ALLOW_THREADS
                    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                                 "mod_wsgi (pid=%d): Adding '%s' to "
                                 "path.", getpid(), value);
                    Py_END_ALLOW_THREADS

                    args = Py_BuildValue("(O)", item);
                    result = PyEval_CallObject(object, args);

                    if (!result) {
                        Py_BEGIN_ALLOW_THREADS
                        ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                                     "mod_wsgi (pid=%d): Call to "
                                     "'site.addsitedir()' failed for '%s', "
                                     "stopping.", getpid(), value);
                        Py_END_ALLOW_THREADS
                    }

                    Py_XDECREF(result);
                    Py_DECREF(item);
                    Py_DECREF(args);

                    end = strchr(start, DELIM);

                    while (result && end) {
#if PY_MAJOR_VERSION >= 3
                        item = PyUnicode_Decode(start, end-start,
                                                Py_FileSystemDefaultEncoding,
                                                "surrogateescape");
#else
                        item = PyString_FromStringAndSize(start, end-start);
#endif
                        start = end+1;

                        value = PyString_AsString(item);

                        Py_BEGIN_ALLOW_THREADS
                        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                                     "mod_wsgi (pid=%d): Adding '%s' to "
                                     "path.", getpid(), value);
                        Py_END_ALLOW_THREADS

                        args = Py_BuildValue("(O)", item);
                        result = PyEval_CallObject(object, args);

                        if (!result) {
                            Py_BEGIN_ALLOW_THREADS
                            ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0),
                                         wsgi_server, "mod_wsgi (pid=%d): "
                                         "Call to 'site.addsitedir()' failed "
                                         "for '%s', stopping.",
                                         getpid(), value);
                            Py_END_ALLOW_THREADS
                        }

                        Py_XDECREF(result);
                        Py_DECREF(item);
                        Py_DECREF(args);

                        end = strchr(start, DELIM);
                    }
                }

                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                             "mod_wsgi (pid=%d): Adding '%s' to "
                             "path.", getpid(), start);
                Py_END_ALLOW_THREADS

                args = Py_BuildValue("(s)", start);
                result = PyEval_CallObject(object, args);

                if (!result) {
                    Py_BEGIN_ALLOW_THREADS
                    ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                                 "mod_wsgi (pid=%d): Call to "
                                 "'site.addsitedir()' failed for '%s'.",
                                 getpid(), start);
                    Py_END_ALLOW_THREADS
                }

                Py_XDECREF(result);
                Py_DECREF(args);

                Py_DECREF(object);
            }
            else {
                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                             "mod_wsgi (pid=%d): Unable to locate "
                             "'site.addsitedir()'.", getpid());
                Py_END_ALLOW_THREADS
            }

            for (i=0; i<PyList_Size(path); i++)
                PyList_Append(tmp, PyList_GetItem(path, i));

            for (i=0; i<PyList_Size(tmp); i++) {
                item = PyList_GetItem(tmp, i);
                if (!PySequence_Contains(old, item)) {
                    int index = PySequence_Index(path, item);
                    PyList_Append(new, item);
                    if (index != -1)
                        PySequence_DelItem(path, index); 
                }
            }

            PyList_SetSlice(path, 0, 0, new);

            Py_DECREF(old);
            Py_DECREF(new);
            Py_DECREF(tmp);
        }
        else {
            if (!module) {
                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                             "mod_wsgi (pid=%d): Unable to import 'site' "
                             "module.", getpid());
                Py_END_ALLOW_THREADS
            }

            if (!path) {
                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_ERR(0), wsgi_server,
                             "mod_wsgi (pid=%d): Lookup for 'sys.path' "
                             "failed.", getpid());
                Py_END_ALLOW_THREADS
            }
        }

        Py_XDECREF(module);
    }

    /*
     * Create 'mod_wsgi' Python module. We first try and import an
     * external Python module of the same name. The intent is
     * that this external module would provide optional features
     * implementable using pure Python code. Don't want to
     * include them in the main Apache mod_wsgi package as that
     * complicates that package and also wouldn't allow them to
     * be released to a separate schedule. It is easier for
     * people to replace Python modules package with a new
     * version than it is to replace Apache module package.
     */

    module = PyImport_ImportModule("mod_wsgi");

    if (!module) {
        PyObject *modules = NULL;

        modules = PyImport_GetModuleDict();
        module = PyDict_GetItemString(modules, "mod_wsgi");

        if (module) {
            PyErr_Print();

            PyDict_DelItemString(modules, "mod_wsgi");
        }

        PyErr_Clear();

        module = PyImport_AddModule("mod_wsgi");

        Py_INCREF(module);
    }
    else if (!*name) {
        Py_BEGIN_ALLOW_THREADS
        ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                     "mod_wsgi (pid=%d): Imported 'mod_wsgi'.",
                     getpid());
        Py_END_ALLOW_THREADS
    }

    /*
     * Add Apache module version information to the Python
     * 'mod_wsgi' module.
     */

    PyModule_AddObject(module, "version", Py_BuildValue("(ii)",
                       MOD_WSGI_MAJORVERSION_NUMBER,
                       MOD_WSGI_MINORVERSION_NUMBER));

    /*
     * Add information about process group and application
     * group to the Python 'mod_wsgi' module.
     */

#if PY_MAJOR_VERSION >= 3
    PyModule_AddObject(module, "process_group",
                       PyUnicode_DecodeLatin1(wsgi_daemon_group,
                       strlen(wsgi_daemon_group), NULL));
    PyModule_AddObject(module, "application_group",
                       PyUnicode_DecodeLatin1(name, strlen(name), NULL));
#else
    PyModule_AddObject(module, "process_group",
                       PyString_FromString(wsgi_daemon_group));
    PyModule_AddObject(module, "application_group",
                       PyString_FromString(name));
#endif

    Py_DECREF(module);

    /*
     * Create 'apache' Python module. If this is not a daemon
     * process and it is the first interpreter created by
     * Python, we first try and import an external Python module
     * of the same name. The intent is that this external module
     * would provide the SWIG bindings for the internal Apache
     * APIs. Only support use of such bindings in the first
     * interpreter created due to threading issues in SWIG
     * generated.
     */

    module = NULL;

    if (!wsgi_daemon_pool) {
        module = PyImport_ImportModule("apache");

        if (!module) {
            PyObject *modules = NULL;

            modules = PyImport_GetModuleDict();
            module = PyDict_GetItemString(modules, "apache");

            if (module) {
                Py_BEGIN_ALLOW_THREADS
                ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                             "mod_wsgi (pid=%d): Unable to import "
                             "'apache' extension module.", getpid());
                Py_END_ALLOW_THREADS

                PyErr_Print();

                PyDict_DelItemString(modules, "apache");

                module = NULL;
            }

            PyErr_Clear();
        }
        else {
            Py_BEGIN_ALLOW_THREADS
            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0), wsgi_server,
                         "mod_wsgi (pid=%d): Imported 'apache'.",
                         getpid());
            Py_END_ALLOW_THREADS
        }
    }

    if (!module) {
        module = PyImport_AddModule("apache");

        Py_INCREF(module);
    }

    /*
     * Add Apache version information to the Python 'apache'
     * module.
     */

    PyModule_AddObject(module, "version", Py_BuildValue("(ii)",
                       AP_SERVER_MAJORVERSION_NUMBER,
                       AP_SERVER_MINORVERSION_NUMBER));

    Py_DECREF(module);

    /*
     * Restore previous thread state. Only need to do
     * this where had to create a new interpreter. This
     * is basically anything except the first Python
     * interpreter instance. We need to restore it in
     * these cases as came into the function holding the
     * simplified GIL state for this thread but creating
     * the interpreter has resulted in a new thread
     * state object being created bound to the newly
     * created interpreter. In doing this though we want
     * to cache the thread state object which has been
     * created when interpreter is created. This is so
     * it can be reused later ensuring that thread local
     * data persists between requests.
     */

    if (self->owner) {
#if APR_HAS_THREADS
        int thread_id = 0;
        int *thread_handle = NULL;

        self->tstate_table = apr_hash_make(wsgi_server->process->pool);

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

        if (wsgi_server_config->verbose_debugging) {
            ap_log_error(APLOG_MARK, WSGI_LOG_DEBUG(0), wsgi_server,
                         "mod_wsgi (pid=%d): Bind thread state for "
                         "thread %d against interpreter '%s'.", getpid(),
                         thread_id, self->name);
        }

        apr_hash_set(self->tstate_table, thread_handle,
                     sizeof(*thread_handle), tstate);

        PyThreadState_Swap(save_tstate);
#else
        self->tstate = tstate;
        PyThreadState_Swap(save_tstate);
#endif
    }

    return self;
}