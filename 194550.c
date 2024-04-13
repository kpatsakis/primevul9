static void qxl_primary_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = qxl_realize_primary;
    k->romfile = "vgabios-qxl.bin";
    k->class_id = PCI_CLASS_DISPLAY_VGA;
    dc->desc = "Spice QXL GPU (primary, vga compatible)";
    dc->hotpluggable = false;
}