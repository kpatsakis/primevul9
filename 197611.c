static inline void gic_update_internal(GICState *s, bool virt)
{
    int best_irq;
    int best_prio;
    int irq_level, fiq_level;
    int cpu, cpu_iface;
    int group = 0;
    qemu_irq *irq_lines = virt ? s->parent_virq : s->parent_irq;
    qemu_irq *fiq_lines = virt ? s->parent_vfiq : s->parent_fiq;

    for (cpu = 0; cpu < s->num_cpu; cpu++) {
        cpu_iface = virt ? (cpu + GIC_NCPU) : cpu;

        s->current_pending[cpu_iface] = 1023;
        if (!gic_irq_signaling_enabled(s, cpu, virt,
                                       GICD_CTLR_EN_GRP0 | GICD_CTLR_EN_GRP1)) {
            qemu_irq_lower(irq_lines[cpu]);
            qemu_irq_lower(fiq_lines[cpu]);
            continue;
        }

        if (virt) {
            gic_get_best_virq(s, cpu, &best_irq, &best_prio, &group);
        } else {
            gic_get_best_irq(s, cpu, &best_irq, &best_prio, &group);
        }

        if (best_irq != 1023) {
            trace_gic_update_bestirq(virt ? "vcpu" : "cpu", cpu,
                                     best_irq, best_prio,
                                     s->priority_mask[cpu_iface],
                                     s->running_priority[cpu_iface]);
        }

        irq_level = fiq_level = 0;

        if (best_prio < s->priority_mask[cpu_iface]) {
            s->current_pending[cpu_iface] = best_irq;
            if (best_prio < s->running_priority[cpu_iface]) {
                if (gic_irq_signaling_enabled(s, cpu, virt, 1 << group)) {
                    if (group == 0 &&
                        s->cpu_ctlr[cpu_iface] & GICC_CTLR_FIQ_EN) {
                        DPRINTF("Raised pending FIQ %d (cpu %d)\n",
                                best_irq, cpu_iface);
                        fiq_level = 1;
                        trace_gic_update_set_irq(cpu, virt ? "vfiq" : "fiq",
                                                 fiq_level);
                    } else {
                        DPRINTF("Raised pending IRQ %d (cpu %d)\n",
                                best_irq, cpu_iface);
                        irq_level = 1;
                        trace_gic_update_set_irq(cpu, virt ? "virq" : "irq",
                                                 irq_level);
                    }
                }
            }
        }

        qemu_set_irq(irq_lines[cpu], irq_level);
        qemu_set_irq(fiq_lines[cpu], fiq_level);
    }
}