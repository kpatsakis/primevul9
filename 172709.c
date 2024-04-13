void btrfs_destroy_dev_replace_tgtdev(struct btrfs_device *tgtdev)
{
	struct btrfs_fs_devices *fs_devices = tgtdev->fs_info->fs_devices;

	WARN_ON(!tgtdev);
	mutex_lock(&fs_devices->device_list_mutex);

	btrfs_sysfs_rm_device_link(fs_devices, tgtdev);

	if (tgtdev->bdev)
		fs_devices->open_devices--;

	fs_devices->num_devices--;

	btrfs_assign_next_active_device(tgtdev, NULL);

	list_del_rcu(&tgtdev->dev_list);

	mutex_unlock(&fs_devices->device_list_mutex);

	/*
	 * The update_dev_time() with in btrfs_scratch_superblocks()
	 * may lead to a call to btrfs_show_devname() which will try
	 * to hold device_list_mutex. And here this device
	 * is already out of device list, so we don't have to hold
	 * the device_list_mutex lock.
	 */
	btrfs_scratch_superblocks(tgtdev->bdev, tgtdev->name->str);

	btrfs_close_bdev(tgtdev);
	call_rcu(&tgtdev->rcu, free_device_rcu);
}