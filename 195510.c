static void usb_ohci_reset_pci(DeviceState *d)
{
    PCIDevice *dev = PCI_DEVICE(d);
    OHCIPCIState *ohci = PCI_OHCI(dev);
    OHCIState *s = &ohci->state;

    ohci_hard_reset(s);
}