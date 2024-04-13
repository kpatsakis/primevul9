static inline int ohci_put_iso_td(OHCIState *ohci,
                                  dma_addr_t addr, struct ohci_iso_td *td)
{
    return put_dwords(ohci, addr, (uint32_t *)td, 4) ||
           put_words(ohci, addr + 16, td->offset, 8);
}