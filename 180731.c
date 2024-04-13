static int rmqueue_bulk(struct zone *zone, unsigned int order,
			unsigned long count, struct list_head *list,
			int migratetype)
{
	int i, alloced = 0;

	spin_lock(&zone->lock);
	for (i = 0; i < count; ++i) {
		struct page *page = __rmqueue(zone, order, migratetype);
		if (unlikely(page == NULL))
			break;

		if (unlikely(check_pcp_refill(page)))
			continue;

		/*
		 * Split buddy pages returned by expand() are received here in
		 * physical page order. The page is added to the tail of
		 * caller's list. From the callers perspective, the linked list
		 * is ordered by page number under some conditions. This is
		 * useful for IO devices that can forward direction from the
		 * head, thus also in the physical page order. This is useful
		 * for IO devices that can merge IO requests if the physical
		 * pages are ordered properly.
		 */
		list_add_tail(&page->lru, list);
		alloced++;
		if (is_migrate_cma(get_pcppage_migratetype(page)))
			__mod_zone_page_state(zone, NR_FREE_CMA_PAGES,
					      -(1 << order));
	}

	/*
	 * i pages were removed from the buddy list even if some leak due
	 * to check_pcp_refill failing so adjust NR_FREE_PAGES based
	 * on i. Do not confuse with 'alloced' which is the number of
	 * pages added to the pcp list.
	 */
	__mod_zone_page_state(zone, NR_FREE_PAGES, -(i << order));
	spin_unlock(&zone->lock);
	return alloced;
}