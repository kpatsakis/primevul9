xfs_log_force_inode(
	struct xfs_inode	*ip)
{
	xfs_lsn_t		lsn = 0;

	xfs_ilock(ip, XFS_ILOCK_SHARED);
	if (xfs_ipincount(ip))
		lsn = ip->i_itemp->ili_last_lsn;
	xfs_iunlock(ip, XFS_ILOCK_SHARED);

	if (!lsn)
		return 0;
	return xfs_log_force_lsn(ip->i_mount, lsn, XFS_LOG_SYNC, NULL);
}