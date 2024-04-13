static uint16_t gic_get_current_pending_irq(GICState *s, int cpu,
                                            MemTxAttrs attrs)
{
    uint16_t pending_irq = s->current_pending[cpu];

    if (pending_irq < GIC_MAXIRQ && gic_has_groups(s)) {
        int group = gic_test_group(s, pending_irq, cpu);

        /* On a GIC without the security extensions, reading this register
         * behaves in the same way as a secure access to a GIC with them.
         */
        bool secure = !gic_cpu_ns_access(s, cpu, attrs);

        if (group == 0 && !secure) {
            /* Group0 interrupts hidden from Non-secure access */
            return 1023;
        }
        if (group == 1 && secure && !(s->cpu_ctlr[cpu] & GICC_CTLR_ACK_CTL)) {
            /* Group1 interrupts only seen by Secure access if
             * AckCtl bit set.
             */
            return 1022;
        }
    }
    return pending_irq;
}