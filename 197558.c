static inline void gic_apr_write_ns_view(GICState *s, int cpu, int regno,
                                         uint32_t value)
{
    /* Write the Nonsecure view of GICC_APR<regno>. */
    switch (GIC_MIN_BPR) {
    case 0:
        if (regno < 2) {
            s->nsapr[regno + 2][cpu] = value;
        }
        break;
    case 1:
        if (regno == 0) {
            s->nsapr[regno + 1][cpu] = value;
        }
        break;
    case 2:
        if (regno == 0) {
            s->nsapr[0][cpu] = deposit32(s->nsapr[0][cpu], 16, 16, value);
        }
        break;
    case 3:
        if (regno == 0) {
            s->nsapr[0][cpu] = deposit32(s->nsapr[0][cpu], 8, 8, value);
        }
        break;
    default:
        g_assert_not_reached();
    }
}