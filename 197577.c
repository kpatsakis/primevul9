static void gic_vmcr_write(GICState *s, uint32_t value, MemTxAttrs attrs)
{
    int vcpu = gic_get_current_vcpu(s);
    uint32_t ctlr;
    uint32_t abpr;
    uint32_t bpr;
    uint32_t prio_mask;

    ctlr = FIELD_EX32(value, GICH_VMCR, VMCCtlr);
    abpr = FIELD_EX32(value, GICH_VMCR, VMABP);
    bpr = FIELD_EX32(value, GICH_VMCR, VMBP);
    prio_mask = FIELD_EX32(value, GICH_VMCR, VMPriMask) << 3;

    gic_set_cpu_control(s, vcpu, ctlr, attrs);
    s->abpr[vcpu] = MAX(abpr, GIC_VIRT_MIN_ABPR);
    s->bpr[vcpu] = MAX(bpr, GIC_VIRT_MIN_BPR);
    gic_set_priority_mask(s, vcpu, prio_mask, attrs);
}