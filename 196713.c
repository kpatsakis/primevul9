xfs_init_percpu_counters(
	struct xfs_mount	*mp)
{
	int		error;

	error = percpu_counter_init(&mp->m_icount, 0, GFP_KERNEL);
	if (error)
		return -ENOMEM;

	error = percpu_counter_init(&mp->m_ifree, 0, GFP_KERNEL);
	if (error)
		goto free_icount;

	error = percpu_counter_init(&mp->m_fdblocks, 0, GFP_KERNEL);
	if (error)
		goto free_ifree;

	return 0;

free_ifree:
	percpu_counter_destroy(&mp->m_ifree);
free_icount:
	percpu_counter_destroy(&mp->m_icount);
	return -ENOMEM;
}