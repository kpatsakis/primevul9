__perform_reclaim(gfp_t gfp_mask, unsigned int order,
					const struct alloc_context *ac)
{
	struct reclaim_state reclaim_state;
	int progress;
	unsigned int noreclaim_flag;

	cond_resched();

	/* We now go into synchronous reclaim */
	cpuset_memory_pressure_bump();
	noreclaim_flag = memalloc_noreclaim_save();
	fs_reclaim_acquire(gfp_mask);
	reclaim_state.reclaimed_slab = 0;
	current->reclaim_state = &reclaim_state;

	progress = try_to_free_pages(ac->zonelist, order, gfp_mask,
								ac->nodemask);

	current->reclaim_state = NULL;
	fs_reclaim_release(gfp_mask);
	memalloc_noreclaim_restore(noreclaim_flag);

	cond_resched();

	return progress;
}