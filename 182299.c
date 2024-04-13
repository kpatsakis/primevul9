static void _vmxnet3_deassert_interrupt_line(VMXNET3State *s, int lidx)
{
    PCIDevice *d = PCI_DEVICE(s);

    /*
     * This function should never be called for MSI(X) interrupts
     * because deassertion never required for message interrupts
     */
    assert(!s->msix_used || !msix_enabled(d));
    /*
     * This function should never be called for MSI(X) interrupts
     * because deassertion never required for message interrupts
     */
    assert(!msi_enabled(d));

    VMW_IRPRN("Deasserting line for interrupt %u", lidx);
    pci_irq_deassert(d);
}