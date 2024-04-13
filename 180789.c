bool has_unmovable_pages(struct zone *zone, struct page *page, int count,
			 int migratetype,
			 bool skip_hwpoisoned_pages)
{
	unsigned long pfn, iter, found;

	/*
	 * For avoiding noise data, lru_add_drain_all() should be called
	 * If ZONE_MOVABLE, the zone never contains unmovable pages
	 */
	if (zone_idx(zone) == ZONE_MOVABLE)
		return false;

	/*
	 * CMA allocations (alloc_contig_range) really need to mark isolate
	 * CMA pageblocks even when they are not movable in fact so consider
	 * them movable here.
	 */
	if (is_migrate_cma(migratetype) &&
			is_migrate_cma(get_pageblock_migratetype(page)))
		return false;

	pfn = page_to_pfn(page);
	for (found = 0, iter = 0; iter < pageblock_nr_pages; iter++) {
		unsigned long check = pfn + iter;

		if (!pfn_valid_within(check))
			continue;

		page = pfn_to_page(check);

		if (PageReserved(page))
			return true;

		/*
		 * Hugepages are not in LRU lists, but they're movable.
		 * We need not scan over tail pages bacause we don't
		 * handle each tail page individually in migration.
		 */
		if (PageHuge(page)) {
			iter = round_up(iter + 1, 1<<compound_order(page)) - 1;
			continue;
		}

		/*
		 * We can't use page_count without pin a page
		 * because another CPU can free compound page.
		 * This check already skips compound tails of THP
		 * because their page->_refcount is zero at all time.
		 */
		if (!page_ref_count(page)) {
			if (PageBuddy(page))
				iter += (1 << page_order(page)) - 1;
			continue;
		}

		/*
		 * The HWPoisoned page may be not in buddy system, and
		 * page_count() is not 0.
		 */
		if (skip_hwpoisoned_pages && PageHWPoison(page))
			continue;

		if (__PageMovable(page))
			continue;

		if (!PageLRU(page))
			found++;
		/*
		 * If there are RECLAIMABLE pages, we need to check
		 * it.  But now, memory offline itself doesn't call
		 * shrink_node_slabs() and it still to be fixed.
		 */
		/*
		 * If the page is not RAM, page_count()should be 0.
		 * we don't need more check. This is an _used_ not-movable page.
		 *
		 * The problematic thing here is PG_reserved pages. PG_reserved
		 * is set to both of a memory hole page and a _used_ kernel
		 * page at boot.
		 */
		if (found > count)
			return true;
	}
	return false;
}