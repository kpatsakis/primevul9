static void gic_set_cpu_control(GICState *s, int cpu, uint32_t value,
                                MemTxAttrs attrs)
{
    uint32_t mask;

    if (gic_cpu_ns_access(s, cpu, attrs)) {
        /* The NS view can only write certain bits in the register;
         * the rest are unchanged
         */
        mask = GICC_CTLR_EN_GRP1;
        if (s->revision == 2) {
            mask |= GICC_CTLR_EOIMODE_NS;
        }
        s->cpu_ctlr[cpu] &= ~mask;
        s->cpu_ctlr[cpu] |= (value << 1) & mask;
    } else {
        if (s->revision == 2) {
            mask = s->security_extn ? GICC_CTLR_V2_S_MASK : GICC_CTLR_V2_MASK;
        } else {
            mask = s->security_extn ? GICC_CTLR_V1_S_MASK : GICC_CTLR_V1_MASK;
        }
        s->cpu_ctlr[cpu] = value & mask;
    }
    DPRINTF("CPU Interface %d: Group0 Interrupts %sabled, "
            "Group1 Interrupts %sabled\n", cpu,
            (s->cpu_ctlr[cpu] & GICC_CTLR_EN_GRP0) ? "En" : "Dis",
            (s->cpu_ctlr[cpu] & GICC_CTLR_EN_GRP1) ? "En" : "Dis");
}