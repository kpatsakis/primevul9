static int btrfs_free_stale_devices(const char *path,
				     struct btrfs_device *skip_device)
{
	struct btrfs_fs_devices *fs_devices, *tmp_fs_devices;
	struct btrfs_device *device, *tmp_device;
	int ret = 0;

	if (path)
		ret = -ENOENT;

	list_for_each_entry_safe(fs_devices, tmp_fs_devices, &fs_uuids, fs_list) {

		mutex_lock(&fs_devices->device_list_mutex);
		list_for_each_entry_safe(device, tmp_device,
					 &fs_devices->devices, dev_list) {
			if (skip_device && skip_device == device)
				continue;
			if (path && !device->name)
				continue;
			if (path && !device_path_matched(path, device))
				continue;
			if (fs_devices->opened) {
				/* for an already deleted device return 0 */
				if (path && ret != 0)
					ret = -EBUSY;
				break;
			}

			/* delete the stale device */
			fs_devices->num_devices--;
			list_del(&device->dev_list);
			btrfs_free_device(device);

			ret = 0;
			if (fs_devices->num_devices == 0)
				break;
		}
		mutex_unlock(&fs_devices->device_list_mutex);

		if (fs_devices->num_devices == 0) {
			btrfs_sysfs_remove_fsid(fs_devices);
			list_del(&fs_devices->fs_list);
			free_fs_devices(fs_devices);
		}
	}

	return ret;
}