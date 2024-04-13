static int __init ext4_init_fs(void)
{
	int i, err;

	ratelimit_state_init(&ext4_mount_msg_ratelimit, 30 * HZ, 64);
	ext4_li_info = NULL;
	mutex_init(&ext4_li_mtx);

	/* Build-time check for flags consistency */
	ext4_check_flag_values();

	for (i = 0; i < EXT4_WQ_HASH_SZ; i++)
		init_waitqueue_head(&ext4__ioend_wq[i]);

	err = ext4_init_es();
	if (err)
		return err;

	err = ext4_init_pending();
	if (err)
		goto out7;

	err = ext4_init_post_read_processing();
	if (err)
		goto out6;

	err = ext4_init_pageio();
	if (err)
		goto out5;

	err = ext4_init_system_zone();
	if (err)
		goto out4;

	err = ext4_init_sysfs();
	if (err)
		goto out3;

	err = ext4_init_mballoc();
	if (err)
		goto out2;
	err = init_inodecache();
	if (err)
		goto out1;
	register_as_ext3();
	register_as_ext2();
	err = register_filesystem(&ext4_fs_type);
	if (err)
		goto out;

	return 0;
out:
	unregister_as_ext2();
	unregister_as_ext3();
	destroy_inodecache();
out1:
	ext4_exit_mballoc();
out2:
	ext4_exit_sysfs();
out3:
	ext4_exit_system_zone();
out4:
	ext4_exit_pageio();
out5:
	ext4_exit_post_read_processing();
out6:
	ext4_exit_pending();
out7:
	ext4_exit_es();

	return err;
}