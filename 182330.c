vmxnet3_cleanup_msi(VMXNET3State *s)
{
    PCIDevice *d = PCI_DEVICE(s);

    msi_uninit(d);
}