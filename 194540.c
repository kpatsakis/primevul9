static void qxl_update_irq(PCIQXLDevice *d)
{
    uint32_t pending = le32_to_cpu(d->ram->int_pending);
    uint32_t mask    = le32_to_cpu(d->ram->int_mask);
    int level = !!(pending & mask);
    pci_set_irq(&d->pci, level);
    qxl_ring_set_dirty(d);
}