static void ohci_realize_pxa(DeviceState *dev, Error **errp)
{
    OHCISysBusState *s = SYSBUS_OHCI(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    /* Cannot fail as we pass NULL for masterbus */
    usb_ohci_init(&s->ohci, dev, s->num_ports, s->dma_offset, NULL, 0,
                  &address_space_memory, &error_abort);
    sysbus_init_irq(sbd, &s->ohci.irq);
    sysbus_init_mmio(sbd, &s->ohci.mem);
}