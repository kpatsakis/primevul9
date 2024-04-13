static void wsgi_python_child_init(apr_pool_t *p)
{
    PyGILState_STATE state;
    PyInterpreterState *interp = NULL;
    PyObject *object = NULL;

    int thread_id = 0;
    int *thread_handle = NULL;

    /* Working with Python, so must acquire GIL. */

    state = PyGILState_Ensure();

    /*
     * Trigger any special Python stuff required after a fork.
     * Only do this though if we were responsible for the
     * initialisation of the Python interpreter in the first
     * place to avoid it being done multiple times. Also only
     * do it if Python was initialised in parent process.
     */

    if (wsgi_python_initialized && !wsgi_python_after_fork)
        PyOS_AfterFork();

    /* Finalise any Python objects required by child process. */

    PyType_Ready(&Log_Type);
    PyType_Ready(&Stream_Type);
    PyType_Ready(&Input_Type);
    PyType_Ready(&Adapter_Type);
    PyType_Ready(&Restricted_Type);
    PyType_Ready(&Interpreter_Type);
    PyType_Ready(&ShutdownInterpreter_Type);
    PyType_Ready(&Dispatch_Type);

#if defined(MOD_WSGI_WITH_AAA_HANDLERS)
    PyType_Ready(&Auth_Type);
#endif

    /* Initialise Python interpreter instance table and lock. */

    wsgi_interpreters = PyDict_New();

#if APR_HAS_THREADS
    apr_thread_mutex_create(&wsgi_interp_lock, APR_THREAD_MUTEX_UNNESTED, p);
    apr_thread_mutex_create(&wsgi_module_lock, APR_THREAD_MUTEX_UNNESTED, p);
#endif

    /*
     * Initialise the key for data related to a thread. At
     * the moment we only record an integer thread ID to be
     * used in lookup table to thread states associated with
     * an interprter.
     */

#if APR_HAS_THREADS
    apr_threadkey_private_create(&wsgi_thread_key, NULL, p);

    thread_id = wsgi_thread_count++;
    thread_handle = (int*)apr_pmemdup(wsgi_server->process->pool,
                                      &thread_id, sizeof(thread_id));
    apr_threadkey_private_set(thread_handle, wsgi_thread_key);
#endif

    /*
     * Cache a reference to the first Python interpreter
     * instance. This interpreter is special as some third party
     * Python modules will only work when used from within this
     * interpreter. This is generally when they use the Python
     * simplified GIL API or otherwise don't use threading API
     * properly. An empty string for name is used to identify
     * the first Python interpreter instance.
     */

    object = (PyObject *)newInterpreterObject(NULL);
    PyDict_SetItemString(wsgi_interpreters, "", object);
    Py_DECREF(object);

    /* Restore the prior thread state and release the GIL. */

    PyGILState_Release(state);

    /* Register cleanups to performed on process shutdown. */

#if AP_SERVER_MAJORVERSION_NUMBER < 2
    ap_register_cleanup(p, NULL, wsgi_python_child_cleanup,
                        ap_null_cleanup);
#else
    apr_pool_cleanup_register(p, NULL, wsgi_python_child_cleanup,
                              apr_pool_cleanup_null);
#endif

    /* Loop through import scripts for this process and load them. */

    if (wsgi_import_list) {
        apr_array_header_t *scripts = NULL;

        WSGIScriptFile *entries;
        WSGIScriptFile *entry;

        int i;

        scripts = wsgi_import_list;
        entries = (WSGIScriptFile *)scripts->elts;

        for (i = 0; i < scripts->nelts; ++i) {
            int l = 0;

            entry = &entries[i];

            if (!strcmp(wsgi_daemon_group, entry->process_group)) {
                InterpreterObject *interp = NULL;
                PyObject *modules = NULL;
                PyObject *module = NULL;
                char *name = NULL;
                int exists = 0;

                interp = wsgi_acquire_interpreter(entry->application_group);

                if (!interp) {
                    ap_log_error(APLOG_MARK, WSGI_LOG_CRIT(0), wsgi_server,
                                  "mod_wsgi (pid=%d): Cannot acquire "
                                  "interpreter '%s'.", getpid(),
                                  entry->application_group);
                }

                /* Calculate the Python module name to be used for script. */

                name = wsgi_module_name(p, entry->handler_script);

                /*
                 * Use a lock around the check to see if the module is
                 * already loaded and the import of the module. Strictly
                 * speaking shouldn't be required at this point.
                 */

#if APR_HAS_THREADS
                Py_BEGIN_ALLOW_THREADS
                apr_thread_mutex_lock(wsgi_module_lock);
                Py_END_ALLOW_THREADS
#endif

                modules = PyImport_GetModuleDict();
                module = PyDict_GetItemString(modules, name);

                Py_XINCREF(module);

                if (module)
                    exists = 1;

                /*
                 * If script reloading is enabled and the module for it has
                 * previously been loaded, see if it has been modified since
                 * the last time it was accessed.
                 */

                if (module && wsgi_server_config->script_reloading) {
                    if (wsgi_reload_required(p, NULL, entry->handler_script,
                                             module, NULL)) {
                        /*
                         * Script file has changed. Only support module
                         * reloading for dispatch scripts. Remove the
                         * module from the modules dictionary before
                         * reloading it again. If code is executing within
                         * the module at the time, the callers reference
                         * count on the module should ensure it isn't
                         * actually destroyed until it is finished.
                         */

                        Py_DECREF(module);
                        module = NULL;

                        PyDict_DelItemString(modules, name);
                    }
                }

                if (!module) {
                    module = wsgi_load_source(p, NULL, name, exists,
                                              entry->handler_script,
                                              entry->process_group,
                                              entry->application_group);

                    if (PyErr_Occurred()) 
                        PyErr_Clear(); 
                }

                /* Safe now to release the module lock. */

#if APR_HAS_THREADS
                apr_thread_mutex_unlock(wsgi_module_lock);
#endif

                /* Cleanup and release interpreter, */

                Py_XDECREF(module);

                wsgi_release_interpreter(interp);
            }
        }
    }
}