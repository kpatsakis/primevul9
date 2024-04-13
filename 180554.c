static inline unsigned long page_to_dma_pfn(struct page *pg)
{
	return mm_to_dma_pfn(page_to_pfn(pg));
}