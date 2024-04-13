myMallocFunc(size_t size)
{
    void *ret;

    ret = xmlMemMalloc(size);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlMemFree(ret);
            return (NULL);
        }
    }
    return (ret);
}