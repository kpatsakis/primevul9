static int open_fs_devices(struct btrfs_fs_devices *fs_devices,
				fmode_t flags, void *holder)
{
	struct btrfs_device *device;
	struct btrfs_device *latest_dev = NULL;
	int ret = 0;

	flags |= FMODE_EXCL;

	list_for_each_entry(device, &fs_devices->devices, dev_list) {
		/* Just open everything we can; ignore failures here */
		if (btrfs_open_one_device(fs_devices, device, flags, holder))
			continue;

		if (!latest_dev ||
		    device->generation > latest_dev->generation)
			latest_dev = device;
	}
	if (fs_devices->open_devices == 0) {
		ret = -EINVAL;
		goto out;
	}
	fs_devices->opened = 1;
	fs_devices->latest_bdev = latest_dev->bdev;
	fs_devices->total_rw_bytes = 0;
out:
	return ret;
}