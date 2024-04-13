void btrfs_init_devices_late(struct btrfs_fs_info *fs_info)
{
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	struct btrfs_device *device;

	while (fs_devices) {
		mutex_lock(&fs_devices->device_list_mutex);
		list_for_each_entry(device, &fs_devices->devices, dev_list)
			device->fs_info = fs_info;
		mutex_unlock(&fs_devices->device_list_mutex);

		fs_devices = fs_devices->seed;
	}
}