static void __setup_per_zone_wmarks(void)
{
	unsigned long pages_min = min_free_kbytes >> (PAGE_SHIFT - 10);
	unsigned long lowmem_pages = 0;
	struct zone *zone;
	unsigned long flags;

	/* Calculate total number of !ZONE_HIGHMEM pages */
	for_each_zone(zone) {
		if (!is_highmem(zone))
			lowmem_pages += zone->managed_pages;
	}

	for_each_zone(zone) {
		u64 tmp;

		spin_lock_irqsave(&zone->lock, flags);
		tmp = (u64)pages_min * zone->managed_pages;
		do_div(tmp, lowmem_pages);
		if (is_highmem(zone)) {
			/*
			 * __GFP_HIGH and PF_MEMALLOC allocations usually don't
			 * need highmem pages, so cap pages_min to a small
			 * value here.
			 *
			 * The WMARK_HIGH-WMARK_LOW and (WMARK_LOW-WMARK_MIN)
			 * deltas control asynch page reclaim, and so should
			 * not be capped for highmem.
			 */
			unsigned long min_pages;

			min_pages = zone->managed_pages / 1024;
			min_pages = clamp(min_pages, SWAP_CLUSTER_MAX, 128UL);
			zone->watermark[WMARK_MIN] = min_pages;
		} else {
			/*
			 * If it's a lowmem zone, reserve a number of pages
			 * proportionate to the zone's size.
			 */
			zone->watermark[WMARK_MIN] = tmp;
		}

		/*
		 * Set the kswapd watermarks distance according to the
		 * scale factor in proportion to available memory, but
		 * ensure a minimum size on small systems.
		 */
		tmp = max_t(u64, tmp >> 2,
			    mult_frac(zone->managed_pages,
				      watermark_scale_factor, 10000));

		zone->watermark[WMARK_LOW]  = min_wmark_pages(zone) + tmp;
		zone->watermark[WMARK_HIGH] = min_wmark_pages(zone) + tmp * 2;

		spin_unlock_irqrestore(&zone->lock, flags);
	}

	/* update totalreserve_pages */
	calculate_totalreserve_pages();
}