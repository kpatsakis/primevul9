static inline uint32_t gic_apr_ns_view(GICState *s, int cpu, int regno)
{
    /* Return the Nonsecure view of GICC_APR<regno>. This is the
     * second half of GICC_NSAPR.
     */
    switch (GIC_MIN_BPR) {
    case 0:
        if (regno < 2) {
            return s->nsapr[regno + 2][cpu];
        }
        break;
    case 1:
        if (regno == 0) {
            return s->nsapr[regno + 1][cpu];
        }
        break;
    case 2:
        if (regno == 0) {
            return extract32(s->nsapr[0][cpu], 16, 16);
        }
        break;
    case 3:
        if (regno == 0) {
            return extract32(s->nsapr[0][cpu], 8, 8);
        }
        break;
    default:
        g_assert_not_reached();
    }
    return 0;
}