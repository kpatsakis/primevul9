int __isolate_free_page(struct page *page, unsigned int order)
{
	unsigned long watermark;
	struct zone *zone;
	int mt;

	BUG_ON(!PageBuddy(page));

	zone = page_zone(page);
	mt = get_pageblock_migratetype(page);

	if (!is_migrate_isolate(mt)) {
		/*
		 * Obey watermarks as if the page was being allocated. We can
		 * emulate a high-order watermark check with a raised order-0
		 * watermark, because we already know our high-order page
		 * exists.
		 */
		watermark = min_wmark_pages(zone) + (1UL << order);
		if (!zone_watermark_ok(zone, 0, watermark, 0, ALLOC_CMA))
			return 0;

		__mod_zone_freepage_state(zone, -(1UL << order), mt);
	}

	/* Remove page from free list */
	list_del(&page->lru);
	zone->free_area[order].nr_free--;
	rmv_page_order(page);

	/*
	 * Set the pageblock if the isolated page is at least half of a
	 * pageblock
	 */
	if (order >= pageblock_order - 1) {
		struct page *endpage = page + (1 << order) - 1;
		for (; page < endpage; page += pageblock_nr_pages) {
			int mt = get_pageblock_migratetype(page);
			if (!is_migrate_isolate(mt) && !is_migrate_cma(mt)
			    && !is_migrate_highatomic(mt))
				set_pageblock_migratetype(page,
							  MIGRATE_MOVABLE);
		}
	}


	return 1UL << order;
}