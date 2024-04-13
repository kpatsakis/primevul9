static void rtl8139_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = pci_rtl8139_realize;
    k->exit = pci_rtl8139_uninit;
    k->romfile = "efi-rtl8139.rom";
    k->vendor_id = PCI_VENDOR_ID_REALTEK;
    k->device_id = PCI_DEVICE_ID_REALTEK_8139;
    k->revision = RTL8139_PCI_REVID; /* >=0x20 is for 8139C+ */
    k->class_id = PCI_CLASS_NETWORK_ETHERNET;
    dc->reset = rtl8139_reset;
    dc->vmsd = &vmstate_rtl8139;
    device_class_set_props(dc, rtl8139_properties);
    set_bit(DEVICE_CATEGORY_NETWORK, dc->categories);
}