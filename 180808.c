should_reclaim_retry(gfp_t gfp_mask, unsigned order,
		     struct alloc_context *ac, int alloc_flags,
		     bool did_some_progress, int *no_progress_loops)
{
	struct zone *zone;
	struct zoneref *z;

	/*
	 * Costly allocations might have made a progress but this doesn't mean
	 * their order will become available due to high fragmentation so
	 * always increment the no progress counter for them
	 */
	if (did_some_progress && order <= PAGE_ALLOC_COSTLY_ORDER)
		*no_progress_loops = 0;
	else
		(*no_progress_loops)++;

	/*
	 * Make sure we converge to OOM if we cannot make any progress
	 * several times in the row.
	 */
	if (*no_progress_loops > MAX_RECLAIM_RETRIES) {
		/* Before OOM, exhaust highatomic_reserve */
		return unreserve_highatomic_pageblock(ac, true);
	}

	/*
	 * Keep reclaiming pages while there is a chance this will lead
	 * somewhere.  If none of the target zones can satisfy our allocation
	 * request even if all reclaimable pages are considered then we are
	 * screwed and have to go OOM.
	 */
	for_each_zone_zonelist_nodemask(zone, z, ac->zonelist, ac->high_zoneidx,
					ac->nodemask) {
		unsigned long available;
		unsigned long reclaimable;
		unsigned long min_wmark = min_wmark_pages(zone);
		bool wmark;

		available = reclaimable = zone_reclaimable_pages(zone);
		available += zone_page_state_snapshot(zone, NR_FREE_PAGES);

		/*
		 * Would the allocation succeed if we reclaimed all
		 * reclaimable pages?
		 */
		wmark = __zone_watermark_ok(zone, order, min_wmark,
				ac_classzone_idx(ac), alloc_flags, available);
		trace_reclaim_retry_zone(z, order, reclaimable,
				available, min_wmark, *no_progress_loops, wmark);
		if (wmark) {
			/*
			 * If we didn't make any progress and have a lot of
			 * dirty + writeback pages then we should wait for
			 * an IO to complete to slow down the reclaim and
			 * prevent from pre mature OOM
			 */
			if (!did_some_progress) {
				unsigned long write_pending;

				write_pending = zone_page_state_snapshot(zone,
							NR_ZONE_WRITE_PENDING);

				if (2 * write_pending > reclaimable) {
					congestion_wait(BLK_RW_ASYNC, HZ/10);
					return true;
				}
			}

			/*
			 * Memory allocation/reclaim might be called from a WQ
			 * context and the current implementation of the WQ
			 * concurrency control doesn't recognize that
			 * a particular WQ is congested if the worker thread is
			 * looping without ever sleeping. Therefore we have to
			 * do a short sleep here rather than calling
			 * cond_resched().
			 */
			if (current->flags & PF_WQ_WORKER)
				schedule_timeout_uninterruptible(1);
			else
				cond_resched();

			return true;
		}
	}

	return false;
}