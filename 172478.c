void btrfs_scratch_superblocks(struct block_device *bdev, const char *device_path)
{
	struct buffer_head *bh;
	struct btrfs_super_block *disk_super;
	int copy_num;

	if (!bdev)
		return;

	for (copy_num = 0; copy_num < BTRFS_SUPER_MIRROR_MAX;
		copy_num++) {

		if (btrfs_read_dev_one_super(bdev, copy_num, &bh))
			continue;

		disk_super = (struct btrfs_super_block *)bh->b_data;

		memset(&disk_super->magic, 0, sizeof(disk_super->magic));
		set_buffer_dirty(bh);
		sync_dirty_buffer(bh);
		brelse(bh);
	}

	/* Notify udev that device has changed */
	btrfs_kobject_uevent(bdev, KOBJ_CHANGE);

	/* Update ctime/mtime for device path for libblkid */
	update_dev_time(device_path);
}