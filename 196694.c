xfs_fs_unfreeze(
	struct super_block	*sb)
{
	struct xfs_mount	*mp = XFS_M(sb);

	xfs_restore_resvblks(mp);
	xfs_log_work_queue(mp);
	xfs_icache_enable_reclaim(mp);
	return 0;
}