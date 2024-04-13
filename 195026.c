static void rtl8139_write_buffer(RTL8139State *s, const void *buf, int size)
{
    PCIDevice *d = PCI_DEVICE(s);

    if (s->RxBufAddr + size > s->RxBufferSize)
    {
        int wrapped = MOD2(s->RxBufAddr + size, s->RxBufferSize);

        /* write packet data */
        if (wrapped && !(s->RxBufferSize < 65536 && rtl8139_RxWrap(s)))
        {
            DPRINTF(">>> rx packet wrapped in buffer at %d\n", size - wrapped);

            if (size > wrapped)
            {
                pci_dma_write(d, s->RxBuf + s->RxBufAddr,
                              buf, size-wrapped);
            }

            /* reset buffer pointer */
            s->RxBufAddr = 0;

            pci_dma_write(d, s->RxBuf + s->RxBufAddr,
                          buf + (size-wrapped), wrapped);

            s->RxBufAddr = wrapped;

            return;
        }
    }

    /* non-wrapping path or overwrapping enabled */
    pci_dma_write(d, s->RxBuf + s->RxBufAddr, buf, size);

    s->RxBufAddr += size;
}