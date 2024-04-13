static inline bool gic_cpu_ns_access(GICState *s, int cpu, MemTxAttrs attrs)
{
    return !gic_is_vcpu(cpu) && s->security_extn && !attrs.secure;
}