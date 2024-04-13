xfs_iunpin(
	struct xfs_inode	*ip)
{
	ASSERT(xfs_isilocked(ip, XFS_ILOCK_EXCL|XFS_ILOCK_SHARED));

	trace_xfs_inode_unpin_nowait(ip, _RET_IP_);

	/* Give the log a push to start the unpinning I/O */
	xfs_log_force_lsn(ip->i_mount, ip->i_itemp->ili_last_lsn, 0, NULL);

}