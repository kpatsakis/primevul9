static int gic_get_group_priority(GICState *s, int cpu, int irq)
{
    /* Return the group priority of the specified interrupt
     * (which is the top bits of its priority, with the number
     * of bits masked determined by the applicable binary point register).
     */
    int bpr;
    uint32_t mask;

    if (gic_has_groups(s) &&
        !(s->cpu_ctlr[cpu] & GICC_CTLR_CBPR) &&
        gic_test_group(s, irq, cpu)) {
        bpr = s->abpr[cpu] - 1;
        assert(bpr >= 0);
    } else {
        bpr = s->bpr[cpu];
    }

    /* a BPR of 0 means the group priority bits are [7:1];
     * a BPR of 1 means they are [7:2], and so on down to
     * a BPR of 7 meaning no group priority bits at all.
     */
    mask = ~0U << ((bpr & 7) + 1);

    return gic_get_priority(s, irq, cpu) & mask;
}