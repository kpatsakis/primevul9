static void qxl_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->vendor_id = REDHAT_PCI_VENDOR_ID;
    k->device_id = QXL_DEVICE_ID_STABLE;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->reset = qxl_reset_handler;
    dc->vmsd = &qxl_vmstate;
    dc->props = qxl_properties;
}