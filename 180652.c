static inline unsigned long dma_to_mm_pfn(unsigned long dma_pfn)
{
	return dma_pfn >> (PAGE_SHIFT - VTD_PAGE_SHIFT);
}