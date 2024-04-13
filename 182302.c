static bool vmxnet3_verify_intx(VMXNET3State *s, int intx)
{
    return s->msix_used || msi_enabled(PCI_DEVICE(s))
        || intx == pci_get_byte(s->parent_obj.config + PCI_INTERRUPT_PIN) - 1;
}