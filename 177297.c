PyImaging_MapperNew(const char* filename, int readonly)
{
    ImagingMapperObject *mapper;

    if (PyType_Ready(&ImagingMapperType) < 0)
        return NULL;

    mapper = PyObject_New(ImagingMapperObject, &ImagingMapperType);
    if (mapper == NULL)
        return NULL;

    mapper->base = NULL;
    mapper->size = mapper->offset = 0;

#ifdef _WIN32
    mapper->hFile = (HANDLE)-1;
    mapper->hMap  = (HANDLE)-1;

    /* FIXME: currently supports readonly mappings only */
    mapper->hFile = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (mapper->hFile == (HANDLE)-1) {
        PyErr_SetString(PyExc_IOError, "cannot open file");
        Py_DECREF(mapper);
        return NULL;
    }

    mapper->hMap = CreateFileMapping(
        mapper->hFile, NULL,
        PAGE_READONLY,
        0, 0, NULL);
    if (mapper->hMap == (HANDLE)-1) {
        CloseHandle(mapper->hFile);
        PyErr_SetString(PyExc_IOError, "cannot map file");
        Py_DECREF(mapper);
        return NULL;
    }

    mapper->base = (char*) MapViewOfFile(
        mapper->hMap,
        FILE_MAP_READ,
        0, 0, 0);

    mapper->size = GetFileSize(mapper->hFile, 0);
#endif

    return mapper;
}