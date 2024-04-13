static inline u64 dma_pte_addr(struct dma_pte *pte)
{
#ifdef CONFIG_64BIT
	return pte->val & VTD_PAGE_MASK;
#else
	/* Must have a full atomic 64-bit read */
	return  __cmpxchg64(&pte->val, 0ULL, 0ULL) & VTD_PAGE_MASK;
#endif
}