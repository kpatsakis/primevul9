static enum oom_constraint constrained_alloc(struct oom_control *oc)
{
	struct zone *zone;
	struct zoneref *z;
	enum zone_type high_zoneidx = gfp_zone(oc->gfp_mask);
	bool cpuset_limited = false;
	int nid;

	if (is_memcg_oom(oc)) {
		oc->totalpages = mem_cgroup_get_limit(oc->memcg) ?: 1;
		return CONSTRAINT_MEMCG;
	}

	/* Default to all available memory */
	oc->totalpages = totalram_pages + total_swap_pages;

	if (!IS_ENABLED(CONFIG_NUMA))
		return CONSTRAINT_NONE;

	if (!oc->zonelist)
		return CONSTRAINT_NONE;
	/*
	 * Reach here only when __GFP_NOFAIL is used. So, we should avoid
	 * to kill current.We have to random task kill in this case.
	 * Hopefully, CONSTRAINT_THISNODE...but no way to handle it, now.
	 */
	if (oc->gfp_mask & __GFP_THISNODE)
		return CONSTRAINT_NONE;

	/*
	 * This is not a __GFP_THISNODE allocation, so a truncated nodemask in
	 * the page allocator means a mempolicy is in effect.  Cpuset policy
	 * is enforced in get_page_from_freelist().
	 */
	if (oc->nodemask &&
	    !nodes_subset(node_states[N_MEMORY], *oc->nodemask)) {
		oc->totalpages = total_swap_pages;
		for_each_node_mask(nid, *oc->nodemask)
			oc->totalpages += node_spanned_pages(nid);
		return CONSTRAINT_MEMORY_POLICY;
	}

	/* Check this allocation failure is caused by cpuset's wall function */
	for_each_zone_zonelist_nodemask(zone, z, oc->zonelist,
			high_zoneidx, oc->nodemask)
		if (!cpuset_zone_allowed(zone, oc->gfp_mask))
			cpuset_limited = true;

	if (cpuset_limited) {
		oc->totalpages = total_swap_pages;
		for_each_node_mask(nid, cpuset_current_mems_allowed)
			oc->totalpages += node_spanned_pages(nid);
		return CONSTRAINT_CPUSET;
	}
	return CONSTRAINT_NONE;
}