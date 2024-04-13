static void vmxnet3_pci_uninit(PCIDevice *pci_dev)
{
    VMXNET3State *s = VMXNET3(pci_dev);

    VMW_CBPRN("Starting uninit...");

    vmxnet3_net_uninit(s);

    vmxnet3_cleanup_msix(s);

    vmxnet3_cleanup_msi(s);
}