__rmqueue(struct zone *zone, unsigned int order, int migratetype)
{
	struct page *page;

retry:
	page = __rmqueue_smallest(zone, order, migratetype);
	if (unlikely(!page)) {
		if (migratetype == MIGRATE_MOVABLE)
			page = __rmqueue_cma_fallback(zone, order);

		if (!page && __rmqueue_fallback(zone, order, migratetype))
			goto retry;
	}

	trace_mm_page_alloc_zone_locked(page, order, migratetype);
	return page;
}