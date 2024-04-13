static void vmxnet3_class_init(ObjectClass *class, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *c = PCI_DEVICE_CLASS(class);
    VMXNET3Class *vc = VMXNET3_DEVICE_CLASS(class);

    c->realize = vmxnet3_pci_realize;
    c->exit = vmxnet3_pci_uninit;
    c->vendor_id = PCI_VENDOR_ID_VMWARE;
    c->device_id = PCI_DEVICE_ID_VMWARE_VMXNET3;
    c->revision = PCI_DEVICE_ID_VMWARE_VMXNET3_REVISION;
    c->romfile = "efi-vmxnet3.rom";
    c->class_id = PCI_CLASS_NETWORK_ETHERNET;
    c->subsystem_vendor_id = PCI_VENDOR_ID_VMWARE;
    c->subsystem_id = PCI_DEVICE_ID_VMWARE_VMXNET3;
    device_class_set_parent_realize(dc, vmxnet3_realize,
                                    &vc->parent_dc_realize);
    dc->desc = "VMWare Paravirtualized Ethernet v3";
    dc->reset = vmxnet3_qdev_reset;
    dc->vmsd = &vmstate_vmxnet3;
    device_class_set_props(dc, vmxnet3_properties);
    set_bit(DEVICE_CATEGORY_NETWORK, dc->categories);
}