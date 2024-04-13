static void gic_activate_irq(GICState *s, int cpu, int irq)
{
    /* Set the appropriate Active Priority Register bit for this IRQ,
     * and update the running priority.
     */
    int prio = gic_get_group_priority(s, cpu, irq);
    int min_bpr = gic_is_vcpu(cpu) ? GIC_VIRT_MIN_BPR : GIC_MIN_BPR;
    int preemption_level = prio >> (min_bpr + 1);
    int regno = preemption_level / 32;
    int bitno = preemption_level % 32;
    uint32_t *papr = NULL;

    if (gic_is_vcpu(cpu)) {
        assert(regno == 0);
        papr = &s->h_apr[gic_get_vcpu_real_id(cpu)];
    } else if (gic_has_groups(s) && gic_test_group(s, irq, cpu)) {
        papr = &s->nsapr[regno][cpu];
    } else {
        papr = &s->apr[regno][cpu];
    }

    *papr |= (1 << bitno);

    s->running_priority[cpu] = prio;
    gic_set_active(s, irq, cpu);
}