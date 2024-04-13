static void ohci_die(OHCIState *ohci)
{
    OHCIPCIState *dev = container_of(ohci, OHCIPCIState, state);

    trace_usb_ohci_die();

    ohci_set_interrupt(ohci, OHCI_INTR_UE);
    ohci_bus_stop(ohci);
    pci_set_word(dev->parent_obj.config + PCI_STATUS,
                 PCI_STATUS_DETECTED_PARITY);
}