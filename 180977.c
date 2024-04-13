static void warn_alloc_show_mem(gfp_t gfp_mask, nodemask_t *nodemask)
{
	unsigned int filter = SHOW_MEM_FILTER_NODES;
	static DEFINE_RATELIMIT_STATE(show_mem_rs, HZ, 1);

	if (should_suppress_show_mem() || !__ratelimit(&show_mem_rs))
		return;

	/*
	 * This documents exceptions given to allocations in certain
	 * contexts that are allowed to allocate outside current's set
	 * of allowed nodes.
	 */
	if (!(gfp_mask & __GFP_NOMEMALLOC))
		if (tsk_is_oom_victim(current) ||
		    (current->flags & (PF_MEMALLOC | PF_EXITING)))
			filter &= ~SHOW_MEM_FILTER_NODES;
	if (in_interrupt() || !(gfp_mask & __GFP_DIRECT_RECLAIM))
		filter &= ~SHOW_MEM_FILTER_NODES;

	show_mem(filter, nodemask);
}