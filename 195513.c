static inline int ohci_read_ed(OHCIState *ohci,
                               dma_addr_t addr, struct ohci_ed *ed)
{
    return get_dwords(ohci, addr, (uint32_t *)ed, sizeof(*ed) >> 2);
}