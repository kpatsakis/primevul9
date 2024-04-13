void btrfs_free_extra_devids(struct btrfs_fs_devices *fs_devices, int step)
{
	struct btrfs_device *device, *next;
	struct btrfs_device *latest_dev = NULL;

	mutex_lock(&uuid_mutex);
again:
	/* This is the initialized path, it is safe to release the devices. */
	list_for_each_entry_safe(device, next, &fs_devices->devices, dev_list) {
		if (test_bit(BTRFS_DEV_STATE_IN_FS_METADATA,
							&device->dev_state)) {
			if (!test_bit(BTRFS_DEV_STATE_REPLACE_TGT,
			     &device->dev_state) &&
			     (!latest_dev ||
			      device->generation > latest_dev->generation)) {
				latest_dev = device;
			}
			continue;
		}

		if (device->devid == BTRFS_DEV_REPLACE_DEVID) {
			/*
			 * In the first step, keep the device which has
			 * the correct fsid and the devid that is used
			 * for the dev_replace procedure.
			 * In the second step, the dev_replace state is
			 * read from the device tree and it is known
			 * whether the procedure is really active or
			 * not, which means whether this device is
			 * used or whether it should be removed.
			 */
			if (step == 0 || test_bit(BTRFS_DEV_STATE_REPLACE_TGT,
						  &device->dev_state)) {
				continue;
			}
		}
		if (device->bdev) {
			blkdev_put(device->bdev, device->mode);
			device->bdev = NULL;
			fs_devices->open_devices--;
		}
		if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
			list_del_init(&device->dev_alloc_list);
			clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
			if (!test_bit(BTRFS_DEV_STATE_REPLACE_TGT,
				      &device->dev_state))
				fs_devices->rw_devices--;
		}
		list_del_init(&device->dev_list);
		fs_devices->num_devices--;
		btrfs_free_device(device);
	}

	if (fs_devices->seed) {
		fs_devices = fs_devices->seed;
		goto again;
	}

	fs_devices->latest_bdev = latest_dev->bdev;

	mutex_unlock(&uuid_mutex);
}