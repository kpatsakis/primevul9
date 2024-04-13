static void qxl_reset_handler(DeviceState *dev)
{
    PCIQXLDevice *d = PCI_QXL(PCI_DEVICE(dev));

    qxl_hard_reset(d, 0);
}