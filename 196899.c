myReallocFunc(void *mem, size_t size)
{
    void *ret;

    ret = xmlMemRealloc(mem, size);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlMemFree(ret);
            return (NULL);
        }
    }
    return (ret);
}