static int __init ext4_init_fs(void)
{
	int i, err;

	ext4_li_info = NULL;
	mutex_init(&ext4_li_mtx);

	ext4_check_flag_values();

	for (i = 0; i < EXT4_WQ_HASH_SZ; i++) {
		mutex_init(&ext4__aio_mutex[i]);
		init_waitqueue_head(&ext4__ioend_wq[i]);
	}

	err = ext4_init_pageio();
	if (err)
		return err;
	err = ext4_init_system_zone();
	if (err)
		goto out6;
	ext4_kset = kset_create_and_add("ext4", NULL, fs_kobj);
	if (!ext4_kset)
		goto out5;
	ext4_proc_root = proc_mkdir("fs/ext4", NULL);

	err = ext4_init_feat_adverts();
	if (err)
		goto out4;

	err = ext4_init_mballoc();
	if (err)
		goto out3;

	err = ext4_init_xattr();
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
	ext4_exit_xattr();
out2:
	ext4_exit_mballoc();
out3:
	ext4_exit_feat_adverts();
out4:
	if (ext4_proc_root)
		remove_proc_entry("fs/ext4", NULL);
	kset_unregister(ext4_kset);
out5:
	ext4_exit_system_zone();
out6:
	ext4_exit_pageio();
	return err;
}