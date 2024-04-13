xfs_quiesce_attr(
	struct xfs_mount	*mp)
{
	int	error = 0;

	/* wait for all modifications to complete */
	while (atomic_read(&mp->m_active_trans) > 0)
		delay(100);

	/* force the log to unpin objects from the now complete transactions */
	xfs_log_force(mp, XFS_LOG_SYNC);

	/* reclaim inodes to do any IO before the freeze completes */
	xfs_reclaim_inodes(mp, 0);
	xfs_reclaim_inodes(mp, SYNC_WAIT);

	/* Push the superblock and write an unmount record */
	error = xfs_log_sbcount(mp);
	if (error)
		xfs_warn(mp, "xfs_attr_quiesce: failed to log sb changes. "
				"Frozen image may not be consistent.");
	/*
	 * Just warn here till VFS can correctly support
	 * read-only remount without racing.
	 */
	WARN_ON(atomic_read(&mp->m_active_trans) != 0);

	xfs_log_quiesce(mp);
}