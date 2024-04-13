static MemTxResult gic_do_hyp_write(void *opaque, hwaddr addr,
                                     uint64_t value, unsigned size,
                                     MemTxAttrs attrs)
{
    GICState **backref = (GICState **)opaque;
    GICState *s = *backref;
    int id = (backref - s->backref);

    return gic_hyp_write(s, id + GIC_NCPU, addr, value, attrs);

}