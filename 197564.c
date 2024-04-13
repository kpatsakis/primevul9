static void gic_deactivate_irq(GICState *s, int cpu, int irq, MemTxAttrs attrs)
{
    int group;

    if (irq >= GIC_MAXIRQ || (!gic_is_vcpu(cpu) && irq >= s->num_irq)) {
        /*
         * This handles two cases:
         * 1. If software writes the ID of a spurious interrupt [ie 1023]
         * to the GICC_DIR, the GIC ignores that write.
         * 2. If software writes the number of a non-existent interrupt
         * this must be a subcase of "value written is not an active interrupt"
         * and so this is UNPREDICTABLE. We choose to ignore it. For vCPUs,
         * all IRQs potentially exist, so this limit does not apply.
         */
        return;
    }

    if (!gic_eoi_split(s, cpu, attrs)) {
        /* This is UNPREDICTABLE; we choose to ignore it */
        qemu_log_mask(LOG_GUEST_ERROR,
                      "gic_deactivate_irq: GICC_DIR write when EOIMode clear");
        return;
    }

    if (gic_is_vcpu(cpu) && !gic_virq_is_valid(s, irq, cpu)) {
        /* This vIRQ does not have an LR entry which is either active or
         * pending and active. Increment EOICount and ignore the write.
         */
        int rcpu = gic_get_vcpu_real_id(cpu);
        s->h_hcr[rcpu] += 1 << R_GICH_HCR_EOICount_SHIFT;

        /* Update the virtual interface in case a maintenance interrupt should
         * be raised.
         */
        gic_update_virt(s);
        return;
    }

    group = gic_has_groups(s) && gic_test_group(s, irq, cpu);

    if (gic_cpu_ns_access(s, cpu, attrs) && !group) {
        DPRINTF("Non-secure DI for Group0 interrupt %d ignored\n", irq);
        return;
    }

    gic_clear_active(s, irq, cpu);
}