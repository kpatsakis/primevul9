xfs_fs_destroy_inode(
	struct inode		*inode)
{
	struct xfs_inode	*ip = XFS_I(inode);

	trace_xfs_destroy_inode(ip);

	ASSERT(!rwsem_is_locked(&inode->i_rwsem));
	XFS_STATS_INC(ip->i_mount, vn_rele);
	XFS_STATS_INC(ip->i_mount, vn_remove);

	xfs_inactive(ip);

	ASSERT(XFS_FORCED_SHUTDOWN(ip->i_mount) || ip->i_delayed_blks == 0);
	XFS_STATS_INC(ip->i_mount, vn_reclaim);

	/*
	 * We should never get here with one of the reclaim flags already set.
	 */
	ASSERT_ALWAYS(!xfs_iflags_test(ip, XFS_IRECLAIMABLE));
	ASSERT_ALWAYS(!xfs_iflags_test(ip, XFS_IRECLAIM));

	/*
	 * We always use background reclaim here because even if the
	 * inode is clean, it still may be under IO and hence we have
	 * to take the flush lock. The background reclaim path handles
	 * this more efficiently than we can here, so simply let background
	 * reclaim tear down all inodes.
	 */
	xfs_inode_set_reclaim_tag(ip);
}