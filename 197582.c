static int gic_get_prio_from_apr_bits(GICState *s, int cpu)
{
    /* Recalculate the current running priority for this CPU based
     * on the set bits in the Active Priority Registers.
     */
    int i;

    if (gic_is_vcpu(cpu)) {
        uint32_t apr = s->h_apr[gic_get_vcpu_real_id(cpu)];
        if (apr) {
            return ctz32(apr) << (GIC_VIRT_MIN_BPR + 1);
        } else {
            return 0x100;
        }
    }

    for (i = 0; i < GIC_NR_APRS; i++) {
        uint32_t apr = s->apr[i][cpu] | s->nsapr[i][cpu];
        if (!apr) {
            continue;
        }
        return (i * 32 + ctz32(apr)) << (GIC_MIN_BPR + 1);
    }
    return 0x100;
}