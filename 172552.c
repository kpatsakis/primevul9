static int btrfs_init_dev_replace_tgtdev(struct btrfs_fs_info *fs_info,
				  const char *device_path,
				  struct btrfs_device *srcdev,
				  struct btrfs_device **device_out)
{
	struct btrfs_device *device;
	struct block_device *bdev;
	struct list_head *devices;
	struct rcu_string *name;
	u64 devid = BTRFS_DEV_REPLACE_DEVID;
	int ret = 0;

	*device_out = NULL;
	if (fs_info->fs_devices->seeding) {
		btrfs_err(fs_info, "the filesystem is a seed filesystem!");
		return -EINVAL;
	}

	bdev = blkdev_get_by_path(device_path, FMODE_WRITE | FMODE_EXCL,
				  fs_info->bdev_holder);
	if (IS_ERR(bdev)) {
		btrfs_err(fs_info, "target device %s is invalid!", device_path);
		return PTR_ERR(bdev);
	}

	filemap_write_and_wait(bdev->bd_inode->i_mapping);

	devices = &fs_info->fs_devices->devices;
	list_for_each_entry(device, devices, dev_list) {
		if (device->bdev == bdev) {
			btrfs_err(fs_info,
				  "target device is in the filesystem!");
			ret = -EEXIST;
			goto error;
		}
	}


	if (i_size_read(bdev->bd_inode) <
	    btrfs_device_get_total_bytes(srcdev)) {
		btrfs_err(fs_info,
			  "target device is smaller than source device!");
		ret = -EINVAL;
		goto error;
	}


	device = btrfs_alloc_device(NULL, &devid, NULL);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		goto error;
	}

	name = rcu_string_strdup(device_path, GFP_KERNEL);
	if (!name) {
		btrfs_free_device(device);
		ret = -ENOMEM;
		goto error;
	}
	rcu_assign_pointer(device->name, name);

	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	set_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
	device->generation = 0;
	device->io_width = fs_info->sectorsize;
	device->io_align = fs_info->sectorsize;
	device->sector_size = fs_info->sectorsize;
	device->total_bytes = btrfs_device_get_total_bytes(srcdev);
	device->disk_total_bytes = btrfs_device_get_disk_total_bytes(srcdev);
	device->bytes_used = btrfs_device_get_bytes_used(srcdev);
	device->commit_total_bytes = srcdev->commit_total_bytes;
	device->commit_bytes_used = device->bytes_used;
	device->fs_info = fs_info;
	device->bdev = bdev;
	set_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &device->dev_state);
	set_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state);
	device->mode = FMODE_EXCL;
	device->dev_stats_valid = 1;
	set_blocksize(device->bdev, BTRFS_BDEV_BLOCKSIZE);
	device->fs_devices = fs_info->fs_devices;
	list_add(&device->dev_list, &fs_info->fs_devices->devices);
	fs_info->fs_devices->num_devices++;
	fs_info->fs_devices->open_devices++;
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);

	*device_out = device;
	return 0;

error:
	blkdev_put(bdev, FMODE_EXCL);
	return ret;
}