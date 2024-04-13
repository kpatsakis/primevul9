xfs_fs_freeze(
	struct super_block	*sb)
{
	struct xfs_mount	*mp = XFS_M(sb);

	xfs_icache_disable_reclaim(mp);
	xfs_save_resvblks(mp);
	xfs_quiesce_attr(mp);
	return xfs_sync_sb(mp, true);
}