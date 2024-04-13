static inline int get_words(OHCIState *ohci,
                            dma_addr_t addr, uint16_t *buf, int num)
{
    int i;

    addr += ohci->localmem_base;

    for (i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
        if (dma_memory_read(ohci->as, addr, buf, sizeof(*buf))) {
            return -1;
        }
        *buf = le16_to_cpu(*buf);
    }

    return 0;
}