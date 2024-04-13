static inline void gic_get_best_irq(GICState *s, int cpu,
                                    int *best_irq, int *best_prio, int *group)
{
    int irq;
    int cm = 1 << cpu;

    *best_irq = 1023;
    *best_prio = 0x100;

    for (irq = 0; irq < s->num_irq; irq++) {
        if (GIC_DIST_TEST_ENABLED(irq, cm) && gic_test_pending(s, irq, cm) &&
            (!GIC_DIST_TEST_ACTIVE(irq, cm)) &&
            (irq < GIC_INTERNAL || GIC_DIST_TARGET(irq) & cm)) {
            if (GIC_DIST_GET_PRIORITY(irq, cpu) < *best_prio) {
                *best_prio = GIC_DIST_GET_PRIORITY(irq, cpu);
                *best_irq = irq;
            }
        }
    }

    if (*best_irq < 1023) {
        *group = GIC_DIST_TEST_GROUP(*best_irq, cm);
    }
}