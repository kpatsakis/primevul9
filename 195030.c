static inline dma_addr_t rtl8139_addr64(uint32_t low, uint32_t high)
{
    return low | ((uint64_t)high << 32);
}