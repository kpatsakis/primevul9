static inline int first_pte_in_page(struct dma_pte *pte)
{
	return !((unsigned long)pte & ~VTD_PAGE_MASK);
}