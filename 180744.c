bool __zone_watermark_ok(struct zone *z, unsigned int order, unsigned long mark,
			 int classzone_idx, unsigned int alloc_flags,
			 long free_pages)
{
	long min = mark;
	int o;
	const bool alloc_harder = (alloc_flags & (ALLOC_HARDER|ALLOC_OOM));

	/* free_pages may go negative - that's OK */
	free_pages -= (1 << order) - 1;

	if (alloc_flags & ALLOC_HIGH)
		min -= min / 2;

	/*
	 * If the caller does not have rights to ALLOC_HARDER then subtract
	 * the high-atomic reserves. This will over-estimate the size of the
	 * atomic reserve but it avoids a search.
	 */
	if (likely(!alloc_harder)) {
		free_pages -= z->nr_reserved_highatomic;
	} else {
		/*
		 * OOM victims can try even harder than normal ALLOC_HARDER
		 * users on the grounds that it's definitely going to be in
		 * the exit path shortly and free memory. Any allocation it
		 * makes during the free path will be small and short-lived.
		 */
		if (alloc_flags & ALLOC_OOM)
			min -= min / 2;
		else
			min -= min / 4;
	}


#ifdef CONFIG_CMA
	/* If allocation can't use CMA areas don't use free CMA pages */
	if (!(alloc_flags & ALLOC_CMA))
		free_pages -= zone_page_state(z, NR_FREE_CMA_PAGES);
#endif

	/*
	 * Check watermarks for an order-0 allocation request. If these
	 * are not met, then a high-order request also cannot go ahead
	 * even if a suitable page happened to be free.
	 */
	if (free_pages <= min + z->lowmem_reserve[classzone_idx])
		return false;

	/* If this is an order-0 request then the watermark is fine */
	if (!order)
		return true;

	/* For a high-order request, check at least one suitable page is free */
	for (o = order; o < MAX_ORDER; o++) {
		struct free_area *area = &z->free_area[o];
		int mt;

		if (!area->nr_free)
			continue;

		for (mt = 0; mt < MIGRATE_PCPTYPES; mt++) {
			if (!list_empty(&area->free_list[mt]))
				return true;
		}

#ifdef CONFIG_CMA
		if ((alloc_flags & ALLOC_CMA) &&
		    !list_empty(&area->free_list[MIGRATE_CMA])) {
			return true;
		}
#endif
		if (alloc_harder &&
			!list_empty(&area->free_list[MIGRATE_HIGHATOMIC]))
			return true;
	}
	return false;
}