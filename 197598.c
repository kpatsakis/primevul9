static uint32_t gic_get_cpu_control(GICState *s, int cpu, MemTxAttrs attrs)
{
    uint32_t ret = s->cpu_ctlr[cpu];

    if (gic_cpu_ns_access(s, cpu, attrs)) {
        /* Construct the NS banked view of GICC_CTLR from the correct
         * bits of the S banked view. We don't need to move the bypass
         * control bits because we don't implement that (IMPDEF) part
         * of the GIC architecture.
         */
        ret = (ret & (GICC_CTLR_EN_GRP1 | GICC_CTLR_EOIMODE_NS)) >> 1;
    }
    return ret;
}