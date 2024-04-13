xfs_fs_sync_fs(
	struct super_block	*sb,
	int			wait)
{
	struct xfs_mount	*mp = XFS_M(sb);

	/*
	 * Doing anything during the async pass would be counterproductive.
	 */
	if (!wait)
		return 0;

	xfs_log_force(mp, XFS_LOG_SYNC);
	if (laptop_mode) {
		/*
		 * The disk must be active because we're syncing.
		 * We schedule log work now (now that the disk is
		 * active) instead of later (when it might not be).
		 */
		flush_delayed_work(&mp->m_log->l_work);
	}

	return 0;
}