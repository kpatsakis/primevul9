static MemTxResult gic_thiscpu_hyp_write(void *opaque, hwaddr addr,
                                     uint64_t value, unsigned size,
                                     MemTxAttrs attrs)
{
    GICState *s = (GICState *)opaque;

    return gic_hyp_write(s, gic_get_current_cpu(s), addr, value, attrs);
}