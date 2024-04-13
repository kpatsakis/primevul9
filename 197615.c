static void gic_drop_prio(GICState *s, int cpu, int group)
{
    /* Drop the priority of the currently active interrupt in the
     * specified group.
     *
     * Note that we can guarantee (because of the requirement to nest
     * GICC_IAR reads [which activate an interrupt and raise priority]
     * with GICC_EOIR writes [which drop the priority for the interrupt])
     * that the interrupt we're being called for is the highest priority
     * active interrupt, meaning that it has the lowest set bit in the
     * APR registers.
     *
     * If the guest does not honour the ordering constraints then the
     * behaviour of the GIC is UNPREDICTABLE, which for us means that
     * the values of the APR registers might become incorrect and the
     * running priority will be wrong, so interrupts that should preempt
     * might not do so, and interrupts that should not preempt might do so.
     */
    if (gic_is_vcpu(cpu)) {
        int rcpu = gic_get_vcpu_real_id(cpu);

        if (s->h_apr[rcpu]) {
            /* Clear lowest set bit */
            s->h_apr[rcpu] &= s->h_apr[rcpu] - 1;
        }
    } else {
        int i;

        for (i = 0; i < GIC_NR_APRS; i++) {
            uint32_t *papr = group ? &s->nsapr[i][cpu] : &s->apr[i][cpu];
            if (!*papr) {
                continue;
            }
            /* Clear lowest set bit */
            *papr &= *papr - 1;
            break;
        }
    }

    s->running_priority[cpu] = gic_get_prio_from_apr_bits(s, cpu);
}