static inline int ohci_read_hcca(OHCIState *ohci,
                                 dma_addr_t addr, struct ohci_hcca *hcca)
{
    return dma_memory_read(ohci->as, addr + ohci->localmem_base,
                           hcca, sizeof(*hcca));
}