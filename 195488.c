static inline int ohci_read_td(OHCIState *ohci,
                               dma_addr_t addr, struct ohci_td *td)
{
    return get_dwords(ohci, addr, (uint32_t *)td, sizeof(*td) >> 2);
}