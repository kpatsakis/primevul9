static void ati_vga_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    dc->reset = ati_vga_reset;
    dc->props = ati_vga_properties;
    dc->hotpluggable = false;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);

    k->class_id = PCI_CLASS_DISPLAY_VGA;
    k->vendor_id = PCI_VENDOR_ID_ATI;
    k->device_id = PCI_DEVICE_ID_ATI_RAGE128_PF;
    k->romfile = "vgabios-ati.bin";
    k->realize = ati_vga_realize;
    k->exit = ati_vga_exit;
}