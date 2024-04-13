static void mptsas_diag_write(void *opaque, hwaddr addr,
                               uint64_t val, unsigned size)
{
    MPTSASState *s = opaque;
    trace_mptsas_diag_write(s, addr, val);
}