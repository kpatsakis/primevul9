static inline int ohci_put_ed(OHCIState *ohci,
                              dma_addr_t addr, struct ohci_ed *ed)
{
    /* ed->tail is under control of the HCD.
     * Since just ed->head is changed by HC, just write back this
     */

    return put_dwords(ohci, addr + ED_WBACK_OFFSET,
                      (uint32_t *)((char *)ed + ED_WBACK_OFFSET),
                      ED_WBACK_SIZE >> 2);
}