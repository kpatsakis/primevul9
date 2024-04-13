static inline bool dma_pte_present(struct dma_pte *pte)
{
	return (pte->val & 3) != 0;
}