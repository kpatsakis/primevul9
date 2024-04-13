static uint64_t mptsas_diag_read(void *opaque, hwaddr addr,
                                   unsigned size)
{
    MPTSASState *s = opaque;
    trace_mptsas_diag_read(s, addr, 0);
    return 0;
}