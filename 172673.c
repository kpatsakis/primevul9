static void btrfs_close_one_device(struct btrfs_device *device)
{
	struct btrfs_fs_devices *fs_devices = device->fs_devices;
	struct btrfs_device *new_device;
	struct rcu_string *name;

	if (device->bdev)
		fs_devices->open_devices--;

	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state) &&
	    device->devid != BTRFS_DEV_REPLACE_DEVID) {
		list_del_init(&device->dev_alloc_list);
		fs_devices->rw_devices--;
	}

	if (test_bit(BTRFS_DEV_STATE_MISSING, &device->dev_state))
		fs_devices->missing_devices--;

	btrfs_close_bdev(device);

	new_device = btrfs_alloc_device(NULL, &device->devid,
					device->uuid);
	BUG_ON(IS_ERR(new_device)); /* -ENOMEM */

	/* Safe because we are under uuid_mutex */
	if (device->name) {
		name = rcu_string_strdup(device->name->str, GFP_NOFS);
		BUG_ON(!name); /* -ENOMEM */
		rcu_assign_pointer(new_device->name, name);
	}

	list_replace_rcu(&device->dev_list, &new_device->dev_list);
	new_device->fs_devices = device->fs_devices;

	call_rcu(&device->rcu, free_device_rcu);
}