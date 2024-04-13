static void __exit ext4_exit_fs(void)
{
	ext4_destroy_lazyinit_thread();
	unregister_as_ext2();
	unregister_as_ext3();
	unregister_filesystem(&ext4_fs_type);
	destroy_inodecache();
	ext4_exit_mballoc();
	ext4_exit_sysfs();
	ext4_exit_system_zone();
	ext4_exit_pageio();
	ext4_exit_post_read_processing();
	ext4_exit_es();
	ext4_exit_pending();
}