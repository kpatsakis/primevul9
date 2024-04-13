static bool gic_eoi_split(GICState *s, int cpu, MemTxAttrs attrs)
{
    if (s->revision != 2) {
        /* Before GICv2 prio-drop and deactivate are not separable */
        return false;
    }
    if (gic_cpu_ns_access(s, cpu, attrs)) {
        return s->cpu_ctlr[cpu] & GICC_CTLR_EOIMODE_NS;
    }
    return s->cpu_ctlr[cpu] & GICC_CTLR_EOIMODE;
}