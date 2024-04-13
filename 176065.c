static void __exit exit_f2fs_fs(void)
{
	remove_proc_entry("fs/f2fs", NULL);
	f2fs_destroy_root_stats();
	unregister_shrinker(&f2fs_shrinker_info);
	unregister_filesystem(&f2fs_fs_type);
	f2fs_exit_crypto();
	destroy_extent_cache();
	destroy_checkpoint_caches();
	destroy_segment_manager_caches();
	destroy_node_manager_caches();
	destroy_inodecache();
	kset_unregister(f2fs_kset);
	f2fs_destroy_trace_ios();
}