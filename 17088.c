heap_available()
{
    long avail = 0;
    void *probes[max_malloc_probes];
    uint n;

    for (n = 0; n < max_malloc_probes; n++) {
        if ((probes[n] = malloc(malloc_probe_size)) == 0)
            break;
        if_debug2('a', "[a]heap_available probe[%d]=0x%lx\n",
                  n, (ulong) probes[n]);
        avail += malloc_probe_size;
    }
    while (n)
        free(probes[--n]);
    return avail;
}