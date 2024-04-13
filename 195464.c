static inline int ohci_put_hcca(OHCIState *ohci,
                                dma_addr_t addr, struct ohci_hcca *hcca)
{
    return dma_memory_write(ohci->as,
                            addr + ohci->localmem_base + HCCA_WRITEBACK_OFFSET,
                            (char *)hcca + HCCA_WRITEBACK_OFFSET,
                            HCCA_WRITEBACK_SIZE);
}