__alloc_pages_may_oom(gfp_t gfp_mask, unsigned int order,
	const struct alloc_context *ac, unsigned long *did_some_progress)
{
	struct oom_control oc = {
		.zonelist = ac->zonelist,
		.nodemask = ac->nodemask,
		.memcg = NULL,
		.gfp_mask = gfp_mask,
		.order = order,
	};
	struct page *page;

	*did_some_progress = 0;

	/*
	 * Acquire the oom lock.  If that fails, somebody else is
	 * making progress for us.
	 */
	if (!mutex_trylock(&oom_lock)) {
		*did_some_progress = 1;
		schedule_timeout_uninterruptible(1);
		return NULL;
	}

	/*
	 * Go through the zonelist yet one more time, keep very high watermark
	 * here, this is only to catch a parallel oom killing, we must fail if
	 * we're still under heavy pressure. But make sure that this reclaim
	 * attempt shall not depend on __GFP_DIRECT_RECLAIM && !__GFP_NORETRY
	 * allocation which will never fail due to oom_lock already held.
	 */
	page = get_page_from_freelist((gfp_mask | __GFP_HARDWALL) &
				      ~__GFP_DIRECT_RECLAIM, order,
				      ALLOC_WMARK_HIGH|ALLOC_CPUSET, ac);
	if (page)
		goto out;

	/* Coredumps can quickly deplete all memory reserves */
	if (current->flags & PF_DUMPCORE)
		goto out;
	/* The OOM killer will not help higher order allocs */
	if (order > PAGE_ALLOC_COSTLY_ORDER)
		goto out;
	/*
	 * We have already exhausted all our reclaim opportunities without any
	 * success so it is time to admit defeat. We will skip the OOM killer
	 * because it is very likely that the caller has a more reasonable
	 * fallback than shooting a random task.
	 */
	if (gfp_mask & __GFP_RETRY_MAYFAIL)
		goto out;
	/* The OOM killer does not needlessly kill tasks for lowmem */
	if (ac->high_zoneidx < ZONE_NORMAL)
		goto out;
	if (pm_suspended_storage())
		goto out;
	/*
	 * XXX: GFP_NOFS allocations should rather fail than rely on
	 * other request to make a forward progress.
	 * We are in an unfortunate situation where out_of_memory cannot
	 * do much for this context but let's try it to at least get
	 * access to memory reserved if the current task is killed (see
	 * out_of_memory). Once filesystems are ready to handle allocation
	 * failures more gracefully we should just bail out here.
	 */

	/* The OOM killer may not free memory on a specific node */
	if (gfp_mask & __GFP_THISNODE)
		goto out;

	/* Exhausted what can be done so it's blamo time */
	if (out_of_memory(&oc) || WARN_ON_ONCE(gfp_mask & __GFP_NOFAIL)) {
		*did_some_progress = 1;

		/*
		 * Help non-failing allocations by giving them access to memory
		 * reserves
		 */
		if (gfp_mask & __GFP_NOFAIL)
			page = __alloc_pages_cpuset_fallback(gfp_mask, order,
					ALLOC_NO_WATERMARKS, ac);
	}
out:
	mutex_unlock(&oom_lock);
	return page;
}