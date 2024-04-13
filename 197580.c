static MemTxResult gic_thisvcpu_read(void *opaque, hwaddr addr, uint64_t *data,
                                    unsigned size, MemTxAttrs attrs)
{
    GICState *s = (GICState *)opaque;

    return gic_cpu_read(s, gic_get_current_vcpu(s), addr, data, attrs);
}