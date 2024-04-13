void gic_dist_set_priority(GICState *s, int cpu, int irq, uint8_t val,
                      MemTxAttrs attrs)
{
    if (s->security_extn && !attrs.secure) {
        if (!GIC_DIST_TEST_GROUP(irq, (1 << cpu))) {
            return; /* Ignore Non-secure access of Group0 IRQ */
        }
        val = 0x80 | (val >> 1); /* Non-secure view */
    }

    val &= gic_fullprio_mask(s, cpu);

    if (irq < GIC_INTERNAL) {
        s->priority1[irq][cpu] = val;
    } else {
        s->priority2[(irq) - GIC_INTERNAL] = val;
    }
}