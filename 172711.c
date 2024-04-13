static int close_fs_devices(struct btrfs_fs_devices *fs_devices)
{
	struct btrfs_device *device, *tmp;

	if (--fs_devices->opened > 0)
		return 0;

	mutex_lock(&fs_devices->device_list_mutex);
	list_for_each_entry_safe(device, tmp, &fs_devices->devices, dev_list) {
		btrfs_close_one_device(device);
	}
	mutex_unlock(&fs_devices->device_list_mutex);

	WARN_ON(fs_devices->open_devices);
	WARN_ON(fs_devices->rw_devices);
	fs_devices->opened = 0;
	fs_devices->seeding = 0;

	return 0;
}