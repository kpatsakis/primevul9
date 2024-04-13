static void gic_update_maintenance(GICState *s)
{
    int cpu = 0;
    int maint_level;

    for (cpu = 0; cpu < s->num_cpu; cpu++) {
        gic_compute_misr(s, cpu);
        maint_level = (s->h_hcr[cpu] & R_GICH_HCR_EN_MASK) && s->h_misr[cpu];

        trace_gic_update_maintenance_irq(cpu, maint_level);
        qemu_set_irq(s->maintenance_irq[cpu], maint_level);
    }
}