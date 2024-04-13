xfs_reinit_percpu_counters(
	struct xfs_mount	*mp)
{
	percpu_counter_set(&mp->m_icount, mp->m_sb.sb_icount);
	percpu_counter_set(&mp->m_ifree, mp->m_sb.sb_ifree);
	percpu_counter_set(&mp->m_fdblocks, mp->m_sb.sb_fdblocks);
}