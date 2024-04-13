static void gic_complete_irq(GICState *s, int cpu, int irq, MemTxAttrs attrs)
{
    int cm = 1 << cpu;
    int group;

    DPRINTF("EOI %d\n", irq);
    if (gic_is_vcpu(cpu)) {
        /* The call to gic_prio_drop() will clear a bit in GICH_APR iff the
         * running prio is < 0x100.
         */
        bool prio_drop = s->running_priority[cpu] < 0x100;

        if (irq >= GIC_MAXIRQ) {
            /* Ignore spurious interrupt */
            return;
        }

        gic_drop_prio(s, cpu, 0);

        if (!gic_eoi_split(s, cpu, attrs)) {
            bool valid = gic_virq_is_valid(s, irq, cpu);
            if (prio_drop && !valid) {
                /* We are in a situation where:
                 *   - V_CTRL.EOIMode is false (no EOI split),
                 *   - The call to gic_drop_prio() cleared a bit in GICH_APR,
                 *   - This vIRQ does not have an LR entry which is either
                 *     active or pending and active.
                 * In that case, we must increment EOICount.
                 */
                int rcpu = gic_get_vcpu_real_id(cpu);
                s->h_hcr[rcpu] += 1 << R_GICH_HCR_EOICount_SHIFT;
            } else if (valid) {
                gic_clear_active(s, irq, cpu);
            }
        }

        gic_update_virt(s);
        return;
    }

    if (irq >= s->num_irq) {
        /* This handles two cases:
         * 1. If software writes the ID of a spurious interrupt [ie 1023]
         * to the GICC_EOIR, the GIC ignores that write.
         * 2. If software writes the number of a non-existent interrupt
         * this must be a subcase of "value written does not match the last
         * valid interrupt value read from the Interrupt Acknowledge
         * register" and so this is UNPREDICTABLE. We choose to ignore it.
         */
        return;
    }
    if (s->running_priority[cpu] == 0x100) {
        return; /* No active IRQ.  */
    }

    if (s->revision == REV_11MPCORE) {
        /* Mark level triggered interrupts as pending if they are still
           raised.  */
        if (!GIC_DIST_TEST_EDGE_TRIGGER(irq) && GIC_DIST_TEST_ENABLED(irq, cm)
            && GIC_DIST_TEST_LEVEL(irq, cm)
            && (GIC_DIST_TARGET(irq) & cm) != 0) {
            DPRINTF("Set %d pending mask %x\n", irq, cm);
            GIC_DIST_SET_PENDING(irq, cm);
        }
    }

    group = gic_has_groups(s) && gic_test_group(s, irq, cpu);

    if (gic_cpu_ns_access(s, cpu, attrs) && !group) {
        DPRINTF("Non-secure EOI for Group0 interrupt %d ignored\n", irq);
        return;
    }

    /* Secure EOI with GICC_CTLR.AckCtl == 0 when the IRQ is a Group 1
     * interrupt is UNPREDICTABLE. We choose to handle it as if AckCtl == 1,
     * i.e. go ahead and complete the irq anyway.
     */

    gic_drop_prio(s, cpu, group);

    /* In GICv2 the guest can choose to split priority-drop and deactivate */
    if (!gic_eoi_split(s, cpu, attrs)) {
        gic_clear_active(s, irq, cpu);
    }
    gic_update(s);
}