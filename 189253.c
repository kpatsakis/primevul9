void* LibRaw::       calloc(size_t n,size_t t)
{
    void *p = memmgr.calloc(n,t);
    return p;
}