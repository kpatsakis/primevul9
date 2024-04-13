static inline int get_dwords(OHCIState *ohci,
                             dma_addr_t addr, uint32_t *buf, int num)
{
    int i;

    addr += ohci->localmem_base;

    for (i = 0; i < num; i++, buf++, addr += sizeof(*buf)) {
        if (dma_memory_read(ohci->as, addr, buf, sizeof(*buf))) {
            return -1;
        }
        *buf = le32_to_cpu(*buf);
    }

    return 0;
}