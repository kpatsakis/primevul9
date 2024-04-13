static inline bool zone_watermark_fast(struct zone *z, unsigned int order,
		unsigned long mark, int classzone_idx, unsigned int alloc_flags)
{
	long free_pages = zone_page_state(z, NR_FREE_PAGES);
	long cma_pages = 0;

#ifdef CONFIG_CMA
	/* If allocation can't use CMA areas don't use free CMA pages */
	if (!(alloc_flags & ALLOC_CMA))
		cma_pages = zone_page_state(z, NR_FREE_CMA_PAGES);
#endif

	/*
	 * Fast check for order-0 only. If this fails then the reserves
	 * need to be calculated. There is a corner case where the check
	 * passes but only the high-order atomic reserve are free. If
	 * the caller is !atomic then it'll uselessly search the free
	 * list. That corner case is then slower but it is harmless.
	 */
	if (!order && (free_pages - cma_pages) > mark + z->lowmem_reserve[classzone_idx])
		return true;

	return __zone_watermark_ok(z, order, mark, classzone_idx, alloc_flags,
					free_pages);
}