myStrdupFunc(const char *str)
{
    char *ret;

    ret = xmlMemoryStrdup(str);
    if (ret != NULL) {
        if (xmlMemUsed() > maxmem) {
            OOM();
            xmlFree(ret);
            return (NULL);
        }
    }
    return (ret);
}