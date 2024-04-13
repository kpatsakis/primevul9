xfs_fs_put_super(
	struct super_block	*sb)
{
	struct xfs_mount	*mp = XFS_M(sb);

	/* if ->fill_super failed, we have no mount to tear down */
	if (!sb->s_fs_info)
		return;

	xfs_notice(mp, "Unmounting Filesystem");
	xfs_filestream_unmount(mp);
	xfs_unmountfs(mp);

	xfs_freesb(mp);
	free_percpu(mp->m_stats.xs_stats);
	xfs_destroy_percpu_counters(mp);
	xfs_destroy_mount_workqueues(mp);
	xfs_close_devices(mp);

	sb->s_fs_info = NULL;
	xfs_free_fsname(mp);
	kfree(mp);
}