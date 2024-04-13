mapping_dealloc(ImagingMapperObject* mapper)
{
#ifdef _WIN32
    if (mapper->base != 0)
        UnmapViewOfFile(mapper->base);
    if (mapper->hMap != (HANDLE)-1)
        CloseHandle(mapper->hMap);
    if (mapper->hFile != (HANDLE)-1)
        CloseHandle(mapper->hFile);
    mapper->base = 0;
    mapper->hMap = mapper->hFile = (HANDLE)-1;
#endif
    PyObject_Del(mapper);
}