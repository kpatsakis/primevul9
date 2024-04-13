static inline unsigned long virt_to_dma_pfn(void *p)
{
	return page_to_dma_pfn(virt_to_page(p));
}