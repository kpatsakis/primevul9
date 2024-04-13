static int btrfs_dev_replace_kthread(void *data)
{
	struct btrfs_fs_info *fs_info = data;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	u64 progress;
	int ret;

	progress = btrfs_dev_replace_progress(fs_info);
	progress = div_u64(progress, 10);
	btrfs_info_in_rcu(fs_info,
		"continuing dev_replace from %s (devid %llu) to target %s @%u%%",
		btrfs_dev_name(dev_replace->srcdev),
		dev_replace->srcdev->devid,
		btrfs_dev_name(dev_replace->tgtdev),
		(unsigned int)progress);

	ret = btrfs_scrub_dev(fs_info, dev_replace->srcdev->devid,
			      dev_replace->committed_cursor_left,
			      btrfs_device_get_total_bytes(dev_replace->srcdev),
			      &dev_replace->scrub_progress, 0, 1);
	ret = btrfs_dev_replace_finishing(fs_info, ret);
	WARN_ON(ret && ret != -ECANCELED);

	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
	return 0;
}