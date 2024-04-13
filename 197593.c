static MemTxResult gic_thisvcpu_write(void *opaque, hwaddr addr,
                                     uint64_t value, unsigned size,
                                     MemTxAttrs attrs)
{
    GICState *s = (GICState *)opaque;

    return gic_cpu_write(s, gic_get_current_vcpu(s), addr, value, attrs);
}