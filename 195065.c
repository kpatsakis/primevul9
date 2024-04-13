static int rtl8139_transmit_one(RTL8139State *s, int descriptor)
{
    if (!rtl8139_transmitter_enabled(s))
    {
        DPRINTF("+++ cannot transmit from descriptor %d: transmitter "
            "disabled\n", descriptor);
        return 0;
    }

    if (s->TxStatus[descriptor] & TxHostOwns)
    {
        DPRINTF("+++ cannot transmit from descriptor %d: owned by host "
            "(%08x)\n", descriptor, s->TxStatus[descriptor]);
        return 0;
    }

    DPRINTF("+++ transmitting from descriptor %d\n", descriptor);

    PCIDevice *d = PCI_DEVICE(s);
    int txsize = s->TxStatus[descriptor] & 0x1fff;
    uint8_t txbuffer[0x2000];

    DPRINTF("+++ transmit reading %d bytes from host memory at 0x%08x\n",
        txsize, s->TxAddr[descriptor]);

    pci_dma_read(d, s->TxAddr[descriptor], txbuffer, txsize);

    /* Mark descriptor as transferred */
    s->TxStatus[descriptor] |= TxHostOwns;
    s->TxStatus[descriptor] |= TxStatOK;

    rtl8139_transfer_frame(s, txbuffer, txsize, 0, NULL);

    DPRINTF("+++ transmitted %d bytes from descriptor %d\n", txsize,
        descriptor);

    /* update interrupt */
    s->IntrStatus |= TxOK;
    rtl8139_update_irq(s);

    return 1;
}