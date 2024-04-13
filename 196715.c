xfs_destroy_percpu_counters(
	struct xfs_mount	*mp)
{
	percpu_counter_destroy(&mp->m_icount);
	percpu_counter_destroy(&mp->m_ifree);
	percpu_counter_destroy(&mp->m_fdblocks);
}