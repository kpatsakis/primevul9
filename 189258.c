void* LibRaw:: malloc(size_t t)
{
    void *p = memmgr.malloc(t);
    return p;
}