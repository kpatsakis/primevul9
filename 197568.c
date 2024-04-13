static void arm_gic_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    ARMGICClass *agc = ARM_GIC_CLASS(klass);

    device_class_set_parent_realize(dc, arm_gic_realize, &agc->parent_realize);
}