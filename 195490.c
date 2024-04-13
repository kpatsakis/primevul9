static inline int ohci_read_iso_td(OHCIState *ohci,
                                   dma_addr_t addr, struct ohci_iso_td *td)
{
    return get_dwords(ohci, addr, (uint32_t *)td, 4) ||
           get_words(ohci, addr + 16, td->offset, 8);
}