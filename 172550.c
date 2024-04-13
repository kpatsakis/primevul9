static int btrfs_open_one_device(struct btrfs_fs_devices *fs_devices,
			struct btrfs_device *device, fmode_t flags,
			void *holder)
{
	struct request_queue *q;
	struct block_device *bdev;
	struct buffer_head *bh;
	struct btrfs_super_block *disk_super;
	u64 devid;
	int ret;

	if (device->bdev)
		return -EINVAL;
	if (!device->name)
		return -EINVAL;

	ret = btrfs_get_bdev_and_sb(device->name->str, flags, holder, 1,
				    &bdev, &bh);
	if (ret)
		return ret;

	disk_super = (struct btrfs_super_block *)bh->b_data;
	devid = btrfs_stack_device_id(&disk_super->dev_item);
	if (devid != device->devid)
		goto error_brelse;

	if (memcmp(device->uuid, disk_super->dev_item.uuid, BTRFS_UUID_SIZE))
		goto error_brelse;

	device->generation = btrfs_super_generation(disk_super);

	if (btrfs_super_flags(disk_super) & BTRFS_SUPER_FLAG_SEEDING) {
		if (btrfs_super_incompat_flags(disk_super) &
		    BTRFS_FEATURE_INCOMPAT_METADATA_UUID) {
			pr_err(
		"BTRFS: Invalid seeding and uuid-changed device detected\n");
			goto error_brelse;
		}

		clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
		fs_devices->seeding = 1;
	} else {
		if (bdev_read_only(bdev))
			clear_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
		else
			set_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
	}

	q = bdev_get_queue(bdev);
	if (!blk_queue_nonrot(q))
		fs_devices->rotating = 1;

	device->bdev = bdev;
	clear_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &device->dev_state);
	device->mode = flags;

	fs_devices->open_devices++;
	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state) &&
	    device->devid != BTRFS_DEV_REPLACE_DEVID) {
		fs_devices->rw_devices++;
		list_add_tail(&device->dev_alloc_list, &fs_devices->alloc_list);
	}
	brelse(bh);

	return 0;

error_brelse:
	brelse(bh);
	blkdev_put(bdev, flags);

	return -EINVAL;
}