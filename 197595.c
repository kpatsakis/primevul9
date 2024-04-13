static inline bool gic_irq_signaling_enabled(GICState *s, int cpu, bool virt,
                                    int group_mask)
{
    int cpu_iface = virt ? (cpu + GIC_NCPU) : cpu;

    if (!virt && !(s->ctlr & group_mask)) {
        return false;
    }

    if (virt && !(s->h_hcr[cpu] & R_GICH_HCR_EN_MASK)) {
        return false;
    }

    if (!(s->cpu_ctlr[cpu_iface] & group_mask)) {
        return false;
    }

    return true;
}