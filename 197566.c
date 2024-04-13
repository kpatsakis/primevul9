static inline uint32_t gic_clear_pending_sgi(GICState *s, int irq, int cpu)
{
    int src;
    uint32_t ret;

    if (!gic_is_vcpu(cpu)) {
        /* Lookup the source CPU for the SGI and clear this in the
         * sgi_pending map.  Return the src and clear the overall pending
         * state on this CPU if the SGI is not pending from any CPUs.
         */
        assert(s->sgi_pending[irq][cpu] != 0);
        src = ctz32(s->sgi_pending[irq][cpu]);
        s->sgi_pending[irq][cpu] &= ~(1 << src);
        if (s->sgi_pending[irq][cpu] == 0) {
            gic_clear_pending(s, irq, cpu);
        }
        ret = irq | ((src & 0x7) << 10);
    } else {
        uint32_t *lr_entry = gic_get_lr_entry(s, irq, cpu);
        src = GICH_LR_CPUID(*lr_entry);

        gic_clear_pending(s, irq, cpu);
        ret = irq | (src << 10);
    }

    return ret;
}