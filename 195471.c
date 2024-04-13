static void ohci_pci_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = usb_ohci_realize_pci;
    k->exit = usb_ohci_exit;
    k->vendor_id = PCI_VENDOR_ID_APPLE;
    k->device_id = PCI_DEVICE_ID_APPLE_IPID_USB;
    k->class_id = PCI_CLASS_SERIAL_USB;
    set_bit(DEVICE_CATEGORY_USB, dc->categories);
    dc->desc = "Apple USB Controller";
    dc->props = ohci_pci_properties;
    dc->hotpluggable = false;
    dc->vmsd = &vmstate_ohci;
    dc->reset = usb_ohci_reset_pci;
}