static void free_pcppages_bulk(struct zone *zone, int count,
					struct per_cpu_pages *pcp)
{
	int migratetype = 0;
	int batch_free = 0;
	bool isolated_pageblocks;

	spin_lock(&zone->lock);
	isolated_pageblocks = has_isolate_pageblock(zone);

	while (count) {
		struct page *page;
		struct list_head *list;

		/*
		 * Remove pages from lists in a round-robin fashion. A
		 * batch_free count is maintained that is incremented when an
		 * empty list is encountered.  This is so more pages are freed
		 * off fuller lists instead of spinning excessively around empty
		 * lists
		 */
		do {
			batch_free++;
			if (++migratetype == MIGRATE_PCPTYPES)
				migratetype = 0;
			list = &pcp->lists[migratetype];
		} while (list_empty(list));

		/* This is the only non-empty list. Free them all. */
		if (batch_free == MIGRATE_PCPTYPES)
			batch_free = count;

		do {
			int mt;	/* migratetype of the to-be-freed page */

			page = list_last_entry(list, struct page, lru);
			/* must delete as __free_one_page list manipulates */
			list_del(&page->lru);

			mt = get_pcppage_migratetype(page);
			/* MIGRATE_ISOLATE page should not go to pcplists */
			VM_BUG_ON_PAGE(is_migrate_isolate(mt), page);
			/* Pageblock could have been isolated meanwhile */
			if (unlikely(isolated_pageblocks))
				mt = get_pageblock_migratetype(page);

			if (bulkfree_pcp_prepare(page))
				continue;

			__free_one_page(page, page_to_pfn(page), zone, 0, mt);
			trace_mm_page_pcpu_drain(page, 0, mt);
		} while (--count && --batch_free && !list_empty(list));
	}
	spin_unlock(&zone->lock);
}