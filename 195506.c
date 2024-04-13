static int ohci_copy_iso_td(OHCIState *ohci,
                            uint32_t start_addr, uint32_t end_addr,
                            uint8_t *buf, int len, DMADirection dir)
{
    dma_addr_t ptr, n;

    ptr = start_addr;
    n = 0x1000 - (ptr & 0xfff);
    if (n > len)
        n = len;

    if (dma_memory_rw(ohci->as, ptr + ohci->localmem_base, buf, n, dir)) {
        return -1;
    }
    if (n == len) {
        return 0;
    }
    ptr = end_addr & ~0xfffu;
    buf += n;
    if (dma_memory_rw(ohci->as, ptr + ohci->localmem_base, buf,
                      len - n, dir)) {
        return -1;
    }
    return 0;
}