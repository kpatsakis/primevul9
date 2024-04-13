static void usb_ohci_realize_pci(PCIDevice *dev, Error **errp)
{
    Error *err = NULL;
    OHCIPCIState *ohci = PCI_OHCI(dev);

    dev->config[PCI_CLASS_PROG] = 0x10; /* OHCI */
    dev->config[PCI_INTERRUPT_PIN] = 0x01; /* interrupt pin A */

    usb_ohci_init(&ohci->state, DEVICE(dev), ohci->num_ports, 0,
                  ohci->masterbus, ohci->firstport,
                  pci_get_address_space(dev), &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }

    ohci->state.irq = pci_allocate_irq(dev);
    pci_register_bar(dev, 0, 0, &ohci->state.mem);
}