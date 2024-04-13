__alloc_pages_direct_compact(gfp_t gfp_mask, unsigned int order,
		unsigned int alloc_flags, const struct alloc_context *ac,
		enum compact_priority prio, enum compact_result *compact_result)
{
	struct page *page;
	unsigned int noreclaim_flag;

	if (!order)
		return NULL;

	noreclaim_flag = memalloc_noreclaim_save();
	*compact_result = try_to_compact_pages(gfp_mask, order, alloc_flags, ac,
									prio);
	memalloc_noreclaim_restore(noreclaim_flag);

	if (*compact_result <= COMPACT_INACTIVE)
		return NULL;

	/*
	 * At least in one zone compaction wasn't deferred or skipped, so let's
	 * count a compaction stall
	 */
	count_vm_event(COMPACTSTALL);

	page = get_page_from_freelist(gfp_mask, order, alloc_flags, ac);

	if (page) {
		struct zone *zone = page_zone(page);

		zone->compact_blockskip_flush = false;
		compaction_defer_reset(zone, order, true);
		count_vm_event(COMPACTSUCCESS);
		return page;
	}

	/*
	 * It's bad if compaction run occurs and fails. The most likely reason
	 * is that pages exist, but not enough to satisfy watermarks.
	 */
	count_vm_event(COMPACTFAIL);

	cond_resched();

	return NULL;
}