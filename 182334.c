static bool _vmxnet3_assert_interrupt_line(VMXNET3State *s, uint32_t int_idx)
{
    PCIDevice *d = PCI_DEVICE(s);

    if (s->msix_used && msix_enabled(d)) {
        VMW_IRPRN("Sending MSI-X notification for vector %u", int_idx);
        msix_notify(d, int_idx);
        return false;
    }
    if (msi_enabled(d)) {
        VMW_IRPRN("Sending MSI notification for vector %u", int_idx);
        msi_notify(d, int_idx);
        return false;
    }

    VMW_IRPRN("Asserting line for interrupt %u", int_idx);
    pci_irq_assert(d);
    return true;
}