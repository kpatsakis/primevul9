__alloc_pages_slowpath(gfp_t gfp_mask, unsigned int order,
						struct alloc_context *ac)
{
	bool can_direct_reclaim = gfp_mask & __GFP_DIRECT_RECLAIM;
	const bool costly_order = order > PAGE_ALLOC_COSTLY_ORDER;
	struct page *page = NULL;
	unsigned int alloc_flags;
	unsigned long did_some_progress;
	enum compact_priority compact_priority;
	enum compact_result compact_result;
	int compaction_retries;
	int no_progress_loops;
	unsigned int cpuset_mems_cookie;
	int reserve_flags;

	/*
	 * In the slowpath, we sanity check order to avoid ever trying to
	 * reclaim >= MAX_ORDER areas which will never succeed. Callers may
	 * be using allocators in order of preference for an area that is
	 * too large.
	 */
	if (order >= MAX_ORDER) {
		WARN_ON_ONCE(!(gfp_mask & __GFP_NOWARN));
		return NULL;
	}

	/*
	 * We also sanity check to catch abuse of atomic reserves being used by
	 * callers that are not in atomic context.
	 */
	if (WARN_ON_ONCE((gfp_mask & (__GFP_ATOMIC|__GFP_DIRECT_RECLAIM)) ==
				(__GFP_ATOMIC|__GFP_DIRECT_RECLAIM)))
		gfp_mask &= ~__GFP_ATOMIC;

retry_cpuset:
	compaction_retries = 0;
	no_progress_loops = 0;
	compact_priority = DEF_COMPACT_PRIORITY;
	cpuset_mems_cookie = read_mems_allowed_begin();

	/*
	 * The fast path uses conservative alloc_flags to succeed only until
	 * kswapd needs to be woken up, and to avoid the cost of setting up
	 * alloc_flags precisely. So we do that now.
	 */
	alloc_flags = gfp_to_alloc_flags(gfp_mask);

	/*
	 * We need to recalculate the starting point for the zonelist iterator
	 * because we might have used different nodemask in the fast path, or
	 * there was a cpuset modification and we are retrying - otherwise we
	 * could end up iterating over non-eligible zones endlessly.
	 */
	ac->preferred_zoneref = first_zones_zonelist(ac->zonelist,
					ac->high_zoneidx, ac->nodemask);
	if (!ac->preferred_zoneref->zone)
		goto nopage;

	if (gfp_mask & __GFP_KSWAPD_RECLAIM)
		wake_all_kswapds(order, ac);

	/*
	 * The adjusted alloc_flags might result in immediate success, so try
	 * that first
	 */
	page = get_page_from_freelist(gfp_mask, order, alloc_flags, ac);
	if (page)
		goto got_pg;

	/*
	 * For costly allocations, try direct compaction first, as it's likely
	 * that we have enough base pages and don't need to reclaim. For non-
	 * movable high-order allocations, do that as well, as compaction will
	 * try prevent permanent fragmentation by migrating from blocks of the
	 * same migratetype.
	 * Don't try this for allocations that are allowed to ignore
	 * watermarks, as the ALLOC_NO_WATERMARKS attempt didn't yet happen.
	 */
	if (can_direct_reclaim &&
			(costly_order ||
			   (order > 0 && ac->migratetype != MIGRATE_MOVABLE))
			&& !gfp_pfmemalloc_allowed(gfp_mask)) {
		page = __alloc_pages_direct_compact(gfp_mask, order,
						alloc_flags, ac,
						INIT_COMPACT_PRIORITY,
						&compact_result);
		if (page)
			goto got_pg;

		/*
		 * Checks for costly allocations with __GFP_NORETRY, which
		 * includes THP page fault allocations
		 */
		if (costly_order && (gfp_mask & __GFP_NORETRY)) {
			/*
			 * If compaction is deferred for high-order allocations,
			 * it is because sync compaction recently failed. If
			 * this is the case and the caller requested a THP
			 * allocation, we do not want to heavily disrupt the
			 * system, so we fail the allocation instead of entering
			 * direct reclaim.
			 */
			if (compact_result == COMPACT_DEFERRED)
				goto nopage;

			/*
			 * Looks like reclaim/compaction is worth trying, but
			 * sync compaction could be very expensive, so keep
			 * using async compaction.
			 */
			compact_priority = INIT_COMPACT_PRIORITY;
		}
	}

retry:
	/* Ensure kswapd doesn't accidentally go to sleep as long as we loop */
	if (gfp_mask & __GFP_KSWAPD_RECLAIM)
		wake_all_kswapds(order, ac);

	reserve_flags = __gfp_pfmemalloc_flags(gfp_mask);
	if (reserve_flags)
		alloc_flags = reserve_flags;

	/*
	 * Reset the zonelist iterators if memory policies can be ignored.
	 * These allocations are high priority and system rather than user
	 * orientated.
	 */
	if (!(alloc_flags & ALLOC_CPUSET) || reserve_flags) {
		ac->zonelist = node_zonelist(numa_node_id(), gfp_mask);
		ac->preferred_zoneref = first_zones_zonelist(ac->zonelist,
					ac->high_zoneidx, ac->nodemask);
	}

	/* Attempt with potentially adjusted zonelist and alloc_flags */
	page = get_page_from_freelist(gfp_mask, order, alloc_flags, ac);
	if (page)
		goto got_pg;

	/* Caller is not willing to reclaim, we can't balance anything */
	if (!can_direct_reclaim)
		goto nopage;

	/* Avoid recursion of direct reclaim */
	if (current->flags & PF_MEMALLOC)
		goto nopage;

	/* Try direct reclaim and then allocating */
	page = __alloc_pages_direct_reclaim(gfp_mask, order, alloc_flags, ac,
							&did_some_progress);
	if (page)
		goto got_pg;

	/* Try direct compaction and then allocating */
	page = __alloc_pages_direct_compact(gfp_mask, order, alloc_flags, ac,
					compact_priority, &compact_result);
	if (page)
		goto got_pg;

	/* Do not loop if specifically requested */
	if (gfp_mask & __GFP_NORETRY)
		goto nopage;

	/*
	 * Do not retry costly high order allocations unless they are
	 * __GFP_RETRY_MAYFAIL
	 */
	if (costly_order && !(gfp_mask & __GFP_RETRY_MAYFAIL))
		goto nopage;

	if (should_reclaim_retry(gfp_mask, order, ac, alloc_flags,
				 did_some_progress > 0, &no_progress_loops))
		goto retry;

	/*
	 * It doesn't make any sense to retry for the compaction if the order-0
	 * reclaim is not able to make any progress because the current
	 * implementation of the compaction depends on the sufficient amount
	 * of free memory (see __compaction_suitable)
	 */
	if (did_some_progress > 0 &&
			should_compact_retry(ac, order, alloc_flags,
				compact_result, &compact_priority,
				&compaction_retries))
		goto retry;


	/* Deal with possible cpuset update races before we start OOM killing */
	if (check_retry_cpuset(cpuset_mems_cookie, ac))
		goto retry_cpuset;

	/* Reclaim has failed us, start killing things */
	page = __alloc_pages_may_oom(gfp_mask, order, ac, &did_some_progress);
	if (page)
		goto got_pg;

	/* Avoid allocations with no watermarks from looping endlessly */
	if (tsk_is_oom_victim(current) &&
	    (alloc_flags == ALLOC_OOM ||
	     (gfp_mask & __GFP_NOMEMALLOC)))
		goto nopage;

	/* Retry as long as the OOM killer is making progress */
	if (did_some_progress) {
		no_progress_loops = 0;
		goto retry;
	}

nopage:
	/* Deal with possible cpuset update races before we fail */
	if (check_retry_cpuset(cpuset_mems_cookie, ac))
		goto retry_cpuset;

	/*
	 * Make sure that __GFP_NOFAIL request doesn't leak out and make sure
	 * we always retry
	 */
	if (gfp_mask & __GFP_NOFAIL) {
		/*
		 * All existing users of the __GFP_NOFAIL are blockable, so warn
		 * of any new users that actually require GFP_NOWAIT
		 */
		if (WARN_ON_ONCE(!can_direct_reclaim))
			goto fail;

		/*
		 * PF_MEMALLOC request from this context is rather bizarre
		 * because we cannot reclaim anything and only can loop waiting
		 * for somebody to do a work for us
		 */
		WARN_ON_ONCE(current->flags & PF_MEMALLOC);

		/*
		 * non failing costly orders are a hard requirement which we
		 * are not prepared for much so let's warn about these users
		 * so that we can identify them and convert them to something
		 * else.
		 */
		WARN_ON_ONCE(order > PAGE_ALLOC_COSTLY_ORDER);

		/*
		 * Help non-failing allocations by giving them access to memory
		 * reserves but do not use ALLOC_NO_WATERMARKS because this
		 * could deplete whole memory reserves which would just make
		 * the situation worse
		 */
		page = __alloc_pages_cpuset_fallback(gfp_mask, order, ALLOC_HARDER, ac);
		if (page)
			goto got_pg;

		cond_resched();
		goto retry;
	}
fail:
	warn_alloc(gfp_mask, ac->nodemask,
			"page allocation failure: order:%u", order);
got_pg:
	return page;
}