exit_xfs_fs(void)
{
	xfs_qm_exit();
	unregister_filesystem(&xfs_fs_type);
#ifdef DEBUG
	xfs_sysfs_del(&xfs_dbg_kobj);
#endif
	xfs_sysfs_del(&xfsstats.xs_kobj);
	free_percpu(xfsstats.xs_stats);
	kset_unregister(xfs_kset);
	xfs_sysctl_unregister();
	xfs_cleanup_procfs();
	xfs_buf_terminate();
	xfs_mru_cache_uninit();
	xfs_destroy_workqueues();
	xfs_destroy_zones();
	xfs_uuid_table_free();
}