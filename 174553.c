static inline void sdhci_update_irq(SDHCIState *s)
{
    qemu_set_irq(s->irq, sdhci_slotint(s));
}