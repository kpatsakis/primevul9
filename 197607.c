uint32_t gic_acknowledge_irq(GICState *s, int cpu, MemTxAttrs attrs)
{
    int ret, irq;

    /* gic_get_current_pending_irq() will return 1022 or 1023 appropriately
     * for the case where this GIC supports grouping and the pending interrupt
     * is in the wrong group.
     */
    irq = gic_get_current_pending_irq(s, cpu, attrs);
    trace_gic_acknowledge_irq(gic_is_vcpu(cpu) ? "vcpu" : "cpu",
                              gic_get_vcpu_real_id(cpu), irq);

    if (irq >= GIC_MAXIRQ) {
        DPRINTF("ACK, no pending interrupt or it is hidden: %d\n", irq);
        return irq;
    }

    if (gic_get_priority(s, irq, cpu) >= s->running_priority[cpu]) {
        DPRINTF("ACK, pending interrupt (%d) has insufficient priority\n", irq);
        return 1023;
    }

    gic_activate_irq(s, cpu, irq);

    if (s->revision == REV_11MPCORE) {
        /* Clear pending flags for both level and edge triggered interrupts.
         * Level triggered IRQs will be reasserted once they become inactive.
         */
        gic_clear_pending(s, irq, cpu);
        ret = irq;
    } else {
        if (irq < GIC_NR_SGIS) {
            ret = gic_clear_pending_sgi(s, irq, cpu);
        } else {
            gic_clear_pending(s, irq, cpu);
            ret = irq;
        }
    }

    if (gic_is_vcpu(cpu)) {
        gic_update_virt(s);
    } else {
        gic_update(s);
    }
    DPRINTF("ACK %d\n", irq);
    return ret;
}