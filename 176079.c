static int __init init_f2fs_fs(void)
{
	int err;

	f2fs_build_trace_ios();

	err = init_inodecache();
	if (err)
		goto fail;
	err = create_node_manager_caches();
	if (err)
		goto free_inodecache;
	err = create_segment_manager_caches();
	if (err)
		goto free_node_manager_caches;
	err = create_checkpoint_caches();
	if (err)
		goto free_segment_manager_caches;
	err = create_extent_cache();
	if (err)
		goto free_checkpoint_caches;
	f2fs_kset = kset_create_and_add("f2fs", NULL, fs_kobj);
	if (!f2fs_kset) {
		err = -ENOMEM;
		goto free_extent_cache;
	}
	err = f2fs_init_crypto();
	if (err)
		goto free_kset;

	err = register_shrinker(&f2fs_shrinker_info);
	if (err)
		goto free_crypto;

	err = register_filesystem(&f2fs_fs_type);
	if (err)
		goto free_shrinker;
	err = f2fs_create_root_stats();
	if (err)
		goto free_filesystem;
	f2fs_proc_root = proc_mkdir("fs/f2fs", NULL);
	return 0;

free_filesystem:
	unregister_filesystem(&f2fs_fs_type);
free_shrinker:
	unregister_shrinker(&f2fs_shrinker_info);
free_crypto:
	f2fs_exit_crypto();
free_kset:
	kset_unregister(f2fs_kset);
free_extent_cache:
	destroy_extent_cache();
free_checkpoint_caches:
	destroy_checkpoint_caches();
free_segment_manager_caches:
	destroy_segment_manager_caches();
free_node_manager_caches:
	destroy_node_manager_caches();
free_inodecache:
	destroy_inodecache();
fail:
	return err;
}