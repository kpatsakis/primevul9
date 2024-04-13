static void gic_set_irq_generic(GICState *s, int irq, int level,
                                int cm, int target)
{
    if (level) {
        GIC_DIST_SET_LEVEL(irq, cm);
        DPRINTF("Set %d pending mask %x\n", irq, target);
        if (GIC_DIST_TEST_EDGE_TRIGGER(irq)) {
            GIC_DIST_SET_PENDING(irq, target);
        }
    } else {
        GIC_DIST_CLEAR_LEVEL(irq, cm);
    }
}