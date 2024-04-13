static void vmxnet3_qdev_reset(DeviceState *dev)
{
    PCIDevice *d = PCI_DEVICE(dev);
    VMXNET3State *s = VMXNET3(d);

    VMW_CBPRN("Starting QDEV reset...");
    vmxnet3_reset(s);
}