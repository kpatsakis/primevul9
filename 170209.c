xfs_ilock_data_map_shared(
	struct xfs_inode	*ip)
{
	uint			lock_mode = XFS_ILOCK_SHARED;

	if (ip->i_df.if_format == XFS_DINODE_FMT_BTREE &&
	    (ip->i_df.if_flags & XFS_IFEXTENTS) == 0)
		lock_mode = XFS_ILOCK_EXCL;
	xfs_ilock(ip, lock_mode);
	return lock_mode;
}