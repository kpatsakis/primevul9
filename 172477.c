struct btrfs_device *btrfs_scan_one_device(const char *path, fmode_t flags,
					   void *holder)
{
	struct btrfs_super_block *disk_super;
	bool new_device_added = false;
	struct btrfs_device *device = NULL;
	struct block_device *bdev;
	struct page *page;
	u64 bytenr;

	lockdep_assert_held(&uuid_mutex);

	/*
	 * we would like to check all the supers, but that would make
	 * a btrfs mount succeed after a mkfs from a different FS.
	 * So, we need to add a special mount option to scan for
	 * later supers, using BTRFS_SUPER_MIRROR_MAX instead
	 */
	bytenr = btrfs_sb_offset(0);
	flags |= FMODE_EXCL;

	bdev = blkdev_get_by_path(path, flags, holder);
	if (IS_ERR(bdev))
		return ERR_CAST(bdev);

	if (btrfs_read_disk_super(bdev, bytenr, &page, &disk_super)) {
		device = ERR_PTR(-EINVAL);
		goto error_bdev_put;
	}

	device = device_list_add(path, disk_super, &new_device_added);
	if (!IS_ERR(device)) {
		if (new_device_added)
			btrfs_free_stale_devices(path, device);
	}

	btrfs_release_disk_super(page);

error_bdev_put:
	blkdev_put(bdev, flags);

	return device;
}