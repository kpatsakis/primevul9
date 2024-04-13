static void rtl8139_update_irq(RTL8139State *s)
{
    PCIDevice *d = PCI_DEVICE(s);
    int isr;
    isr = (s->IntrStatus & s->IntrMask) & 0xffff;

    DPRINTF("Set IRQ to %d (%04x %04x)\n", isr ? 1 : 0, s->IntrStatus,
        s->IntrMask);

    pci_set_irq(d, (isr != 0));
}