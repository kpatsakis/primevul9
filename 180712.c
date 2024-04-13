static inline bool dma_pte_superpage(struct dma_pte *pte)
{
	return (pte->val & DMA_PTE_LARGE_PAGE);
}