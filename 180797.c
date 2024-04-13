should_compact_retry(struct alloc_context *ac, int order, int alloc_flags,
		     enum compact_result compact_result,
		     enum compact_priority *compact_priority,
		     int *compaction_retries)
{
	int max_retries = MAX_COMPACT_RETRIES;
	int min_priority;
	bool ret = false;
	int retries = *compaction_retries;
	enum compact_priority priority = *compact_priority;

	if (!order)
		return false;

	if (compaction_made_progress(compact_result))
		(*compaction_retries)++;

	/*
	 * compaction considers all the zone as desperately out of memory
	 * so it doesn't really make much sense to retry except when the
	 * failure could be caused by insufficient priority
	 */
	if (compaction_failed(compact_result))
		goto check_priority;

	/*
	 * make sure the compaction wasn't deferred or didn't bail out early
	 * due to locks contention before we declare that we should give up.
	 * But do not retry if the given zonelist is not suitable for
	 * compaction.
	 */
	if (compaction_withdrawn(compact_result)) {
		ret = compaction_zonelist_suitable(ac, order, alloc_flags);
		goto out;
	}

	/*
	 * !costly requests are much more important than __GFP_RETRY_MAYFAIL
	 * costly ones because they are de facto nofail and invoke OOM
	 * killer to move on while costly can fail and users are ready
	 * to cope with that. 1/4 retries is rather arbitrary but we
	 * would need much more detailed feedback from compaction to
	 * make a better decision.
	 */
	if (order > PAGE_ALLOC_COSTLY_ORDER)
		max_retries /= 4;
	if (*compaction_retries <= max_retries) {
		ret = true;
		goto out;
	}

	/*
	 * Make sure there are attempts at the highest priority if we exhausted
	 * all retries or failed at the lower priorities.
	 */
check_priority:
	min_priority = (order > PAGE_ALLOC_COSTLY_ORDER) ?
			MIN_COMPACT_COSTLY_PRIORITY : MIN_COMPACT_PRIORITY;

	if (*compact_priority > min_priority) {
		(*compact_priority)--;
		*compaction_retries = 0;
		ret = true;
	}
out:
	trace_compact_retry(order, priority, compact_result, retries, max_retries, ret);
	return ret;
}