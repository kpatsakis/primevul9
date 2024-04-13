static inline bool prepare_alloc_pages(gfp_t gfp_mask, unsigned int order,
		int preferred_nid, nodemask_t *nodemask,
		struct alloc_context *ac, gfp_t *alloc_mask,
		unsigned int *alloc_flags)
{
	ac->high_zoneidx = gfp_zone(gfp_mask);
	ac->zonelist = node_zonelist(preferred_nid, gfp_mask);
	ac->nodemask = nodemask;
	ac->migratetype = gfpflags_to_migratetype(gfp_mask);

	if (cpusets_enabled()) {
		*alloc_mask |= __GFP_HARDWALL;
		if (!ac->nodemask)
			ac->nodemask = &cpuset_current_mems_allowed;
		else
			*alloc_flags |= ALLOC_CPUSET;
	}

	fs_reclaim_acquire(gfp_mask);
	fs_reclaim_release(gfp_mask);

	might_sleep_if(gfp_mask & __GFP_DIRECT_RECLAIM);

	if (should_fail_alloc_page(gfp_mask, order))
		return false;

	if (IS_ENABLED(CONFIG_CMA) && ac->migratetype == MIGRATE_MOVABLE)
		*alloc_flags |= ALLOC_CMA;

	return true;
}