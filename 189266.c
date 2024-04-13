void* LibRaw:: realloc(void *q,size_t t)
{
    void *p = memmgr.realloc(q,t);
    return p;
}