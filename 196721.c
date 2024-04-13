init_xfs_fs(void)
{
	int			error;

	xfs_check_ondisk_structs();

	printk(KERN_INFO XFS_VERSION_STRING " with "
			 XFS_BUILD_OPTIONS " enabled\n");

	xfs_extent_free_init_defer_op();
	xfs_rmap_update_init_defer_op();
	xfs_refcount_update_init_defer_op();
	xfs_bmap_update_init_defer_op();

	xfs_dir_startup();

	error = xfs_init_zones();
	if (error)
		goto out;

	error = xfs_init_workqueues();
	if (error)
		goto out_destroy_zones;

	error = xfs_mru_cache_init();
	if (error)
		goto out_destroy_wq;

	error = xfs_buf_init();
	if (error)
		goto out_mru_cache_uninit;

	error = xfs_init_procfs();
	if (error)
		goto out_buf_terminate;

	error = xfs_sysctl_register();
	if (error)
		goto out_cleanup_procfs;

	xfs_kset = kset_create_and_add("xfs", NULL, fs_kobj);
	if (!xfs_kset) {
		error = -ENOMEM;
		goto out_sysctl_unregister;
	}

	xfsstats.xs_kobj.kobject.kset = xfs_kset;

	xfsstats.xs_stats = alloc_percpu(struct xfsstats);
	if (!xfsstats.xs_stats) {
		error = -ENOMEM;
		goto out_kset_unregister;
	}

	error = xfs_sysfs_init(&xfsstats.xs_kobj, &xfs_stats_ktype, NULL,
			       "stats");
	if (error)
		goto out_free_stats;

#ifdef DEBUG
	xfs_dbg_kobj.kobject.kset = xfs_kset;
	error = xfs_sysfs_init(&xfs_dbg_kobj, &xfs_dbg_ktype, NULL, "debug");
	if (error)
		goto out_remove_stats_kobj;
#endif

	error = xfs_qm_init();
	if (error)
		goto out_remove_dbg_kobj;

	error = register_filesystem(&xfs_fs_type);
	if (error)
		goto out_qm_exit;
	return 0;

 out_qm_exit:
	xfs_qm_exit();
 out_remove_dbg_kobj:
#ifdef DEBUG
	xfs_sysfs_del(&xfs_dbg_kobj);
 out_remove_stats_kobj:
#endif
	xfs_sysfs_del(&xfsstats.xs_kobj);
 out_free_stats:
	free_percpu(xfsstats.xs_stats);
 out_kset_unregister:
	kset_unregister(xfs_kset);
 out_sysctl_unregister:
	xfs_sysctl_unregister();
 out_cleanup_procfs:
	xfs_cleanup_procfs();
 out_buf_terminate:
	xfs_buf_terminate();
 out_mru_cache_uninit:
	xfs_mru_cache_uninit();
 out_destroy_wq:
	xfs_destroy_workqueues();
 out_destroy_zones:
	xfs_destroy_zones();
 out:
	return error;
}