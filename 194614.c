static void qxl_secondary_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = qxl_realize_secondary;
    k->class_id = PCI_CLASS_DISPLAY_OTHER;
    dc->desc = "Spice QXL GPU (secondary)";
}