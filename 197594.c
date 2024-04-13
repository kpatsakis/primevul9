static void arm_gic_realize(DeviceState *dev, Error **errp)
{
    /* Device instance realize function for the GIC sysbus device */
    int i;
    GICState *s = ARM_GIC(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    ARMGICClass *agc = ARM_GIC_GET_CLASS(s);
    Error *local_err = NULL;

    agc->parent_realize(dev, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    if (kvm_enabled() && !kvm_arm_supports_user_irq()) {
        error_setg(errp, "KVM with user space irqchip only works when the "
                         "host kernel supports KVM_CAP_ARM_USER_IRQ");
        return;
    }

    if (s->n_prio_bits > GIC_MAX_PRIORITY_BITS ||
       (s->virt_extn ? s->n_prio_bits < GIC_VIRT_MAX_GROUP_PRIO_BITS :
        s->n_prio_bits < GIC_MIN_PRIORITY_BITS)) {
        error_setg(errp, "num-priority-bits cannot be greater than %d"
                   " or less than %d", GIC_MAX_PRIORITY_BITS,
                   s->virt_extn ? GIC_VIRT_MAX_GROUP_PRIO_BITS :
                   GIC_MIN_PRIORITY_BITS);
        return;
    }

    /* This creates distributor, main CPU interface (s->cpuiomem[0]) and if
     * enabled, virtualization extensions related interfaces (main virtual
     * interface (s->vifaceiomem[0]) and virtual CPU interface).
     */
    gic_init_irqs_and_mmio(s, gic_set_irq, gic_ops, gic_virt_ops);

    /* Extra core-specific regions for the CPU interfaces. This is
     * necessary for "franken-GIC" implementations, for example on
     * Exynos 4.
     * NB that the memory region size of 0x100 applies for the 11MPCore
     * and also cores following the GIC v1 spec (ie A9).
     * GIC v2 defines a larger memory region (0x1000) so this will need
     * to be extended when we implement A15.
     */
    for (i = 0; i < s->num_cpu; i++) {
        s->backref[i] = s;
        memory_region_init_io(&s->cpuiomem[i+1], OBJECT(s), &gic_cpu_ops,
                              &s->backref[i], "gic_cpu", 0x100);
        sysbus_init_mmio(sbd, &s->cpuiomem[i+1]);
    }

    /* Extra core-specific regions for virtual interfaces. This is required by
     * the GICv2 specification.
     */
    if (s->virt_extn) {
        for (i = 0; i < s->num_cpu; i++) {
            memory_region_init_io(&s->vifaceiomem[i + 1], OBJECT(s),
                                  &gic_viface_ops, &s->backref[i],
                                  "gic_viface", 0x200);
            sysbus_init_mmio(sbd, &s->vifaceiomem[i + 1]);
        }
    }

}