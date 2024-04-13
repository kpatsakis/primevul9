static inline unsigned long mm_to_dma_pfn(unsigned long mm_pfn)
{
	return mm_pfn << (PAGE_SHIFT - VTD_PAGE_SHIFT);
}