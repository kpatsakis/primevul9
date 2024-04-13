static struct btrfs_fs_devices *clone_fs_devices(struct btrfs_fs_devices *orig)
{
	struct btrfs_fs_devices *fs_devices;
	struct btrfs_device *device;
	struct btrfs_device *orig_dev;

	fs_devices = alloc_fs_devices(orig->fsid, NULL);
	if (IS_ERR(fs_devices))
		return fs_devices;

	mutex_lock(&orig->device_list_mutex);
	fs_devices->total_devices = orig->total_devices;

	/* We have held the volume lock, it is safe to get the devices. */
	list_for_each_entry(orig_dev, &orig->devices, dev_list) {
		struct rcu_string *name;

		device = btrfs_alloc_device(NULL, &orig_dev->devid,
					    orig_dev->uuid);
		if (IS_ERR(device))
			goto error;

		/*
		 * This is ok to do without rcu read locked because we hold the
		 * uuid mutex so nothing we touch in here is going to disappear.
		 */
		if (orig_dev->name) {
			name = rcu_string_strdup(orig_dev->name->str,
					GFP_KERNEL);
			if (!name) {
				btrfs_free_device(device);
				goto error;
			}
			rcu_assign_pointer(device->name, name);
		}

		list_add(&device->dev_list, &fs_devices->devices);
		device->fs_devices = fs_devices;
		fs_devices->num_devices++;
	}
	mutex_unlock(&orig->device_list_mutex);
	return fs_devices;
error:
	mutex_unlock(&orig->device_list_mutex);
	free_fs_devices(fs_devices);
	return ERR_PTR(-ENOMEM);
}