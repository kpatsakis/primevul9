static inline int gic_get_current_vcpu(GICState *s)
{
    return gic_get_current_cpu(s) + GIC_NCPU;
}