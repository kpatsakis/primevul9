static inline void dma_clear_pte(struct dma_pte *pte)
{
	pte->val = 0;
}