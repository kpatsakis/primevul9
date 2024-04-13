static uint32_t gic_get_priority_mask(GICState *s, int cpu, MemTxAttrs attrs)
{
    uint32_t pmask = s->priority_mask[cpu];

    if (gic_cpu_ns_access(s, cpu, attrs)) {
        if (pmask & 0x80) {
            /* Priority Mask in upper half, return Non-secure view */
            pmask = (pmask << 1) & 0xff;
        } else {
            /* Priority Mask in lower half, RAZ */
            pmask = 0;
        }
    }
    return pmask;
}