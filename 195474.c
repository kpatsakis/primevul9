static inline int put_words(OHCIState *ohci,
                            dma_addr_t addr, uint16_t *buf, int num)
{
    int i;

    addr += ohci->localmem_base;

    for (i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
        uint16_t tmp = cpu_to_le16(*buf);
        if (dma_memory_write(ohci->as, addr, &tmp, sizeof(tmp))) {
            return -1;
        }
    }

    return 0;
}