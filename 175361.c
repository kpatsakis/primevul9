static unsigned int** get_qtables_arrays(PyObject* qtables) {
    PyObject* tables;
    PyObject* table;
    PyObject* table_data;
    int i, j, num_tables;
    unsigned int **qarrays;

    if ((qtables ==  NULL) || (qtables == Py_None)) {
        return NULL;
    }

    if (!PySequence_Check(qtables)) {
        PyErr_SetString(PyExc_ValueError, "Invalid quantization tables");
        return NULL;
    }

    tables = PySequence_Fast(qtables, "expected a sequence");
    num_tables = PySequence_Size(qtables);
    if (num_tables < 2 || num_tables > NUM_QUANT_TBLS) {
        PyErr_SetString(PyExc_ValueError, "Not a valid numbers of quantization tables. Should be between 2 and 4.");
        return NULL;
    }
    qarrays = (unsigned int**) PyMem_Malloc(num_tables * sizeof(unsigned int));
    if (!qarrays) {
        Py_DECREF(tables);
        PyErr_NoMemory();
        return NULL;
    }
    for (i = 0; i < num_tables; i++) {
        table = PySequence_Fast_GET_ITEM(tables, i);
        if (!PySequence_Check(table)) {
            Py_DECREF(tables);
            PyErr_SetString(PyExc_ValueError, "Invalid quantization tables");
            return NULL;
        }
        if (PySequence_Size(table) != DCTSIZE2) {
            Py_DECREF(tables);
            PyErr_SetString(PyExc_ValueError, "Invalid quantization tables");
            return NULL;
        }
        table_data = PySequence_Fast(table, "expected a sequence");
        qarrays[i] = (unsigned int*) PyMem_Malloc(DCTSIZE2 * sizeof(unsigned int));
        if (!qarrays[i]) {
            Py_DECREF(tables);
            PyErr_NoMemory();
            return NULL;
        }
        for (j = 0; j < DCTSIZE2; j++) {
            qarrays[i][j] = PyInt_AS_LONG(PySequence_Fast_GET_ITEM(table_data, j));
        }
    }

    Py_DECREF(tables);

    if (PyErr_Occurred()) {
        PyMem_Free(qarrays);
        qarrays = NULL;
    }

    return qarrays;
}