static MemTxResult gic_do_hyp_read(void *opaque, hwaddr addr, uint64_t *data,
                                    unsigned size, MemTxAttrs attrs)
{
    GICState **backref = (GICState **)opaque;
    GICState *s = *backref;
    int id = (backref - s->backref);

    return gic_hyp_read(s, id, addr, data, attrs);
}