static int btrfs_dev_replace_start(struct btrfs_fs_info *fs_info,
		const char *tgtdev_name, u64 srcdevid, const char *srcdev_name,
		int read_src)
{
	struct btrfs_root *root = fs_info->dev_root;
	struct btrfs_trans_handle *trans;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	int ret;
	struct btrfs_device *tgt_device = NULL;
	struct btrfs_device *src_device = NULL;
	bool need_unlock;

	src_device = btrfs_find_device_by_devspec(fs_info, srcdevid,
						  srcdev_name);
	if (IS_ERR(src_device))
		return PTR_ERR(src_device);

	if (btrfs_pinned_by_swapfile(fs_info, src_device)) {
		btrfs_warn_in_rcu(fs_info,
	  "cannot replace device %s (devid %llu) due to active swapfile",
			btrfs_dev_name(src_device), src_device->devid);
		return -ETXTBSY;
	}

	ret = btrfs_init_dev_replace_tgtdev(fs_info, tgtdev_name,
					    src_device, &tgt_device);
	if (ret)
		return ret;

	/*
	 * Here we commit the transaction to make sure commit_total_bytes
	 * of all the devices are updated.
	 */
	trans = btrfs_attach_transaction(root);
	if (!IS_ERR(trans)) {
		ret = btrfs_commit_transaction(trans);
		if (ret)
			return ret;
	} else if (PTR_ERR(trans) != -ENOENT) {
		return PTR_ERR(trans);
	}

	need_unlock = true;
	down_write(&dev_replace->rwsem);
	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		ASSERT(0);
		ret = BTRFS_IOCTL_DEV_REPLACE_RESULT_ALREADY_STARTED;
		goto leave;
	}

	dev_replace->cont_reading_from_srcdev_mode = read_src;
	WARN_ON(!src_device);
	dev_replace->srcdev = src_device;
	dev_replace->tgtdev = tgt_device;

	btrfs_info_in_rcu(fs_info,
		      "dev_replace from %s (devid %llu) to %s started",
		      btrfs_dev_name(src_device),
		      src_device->devid,
		      rcu_str_deref(tgt_device->name));

	/*
	 * from now on, the writes to the srcdev are all duplicated to
	 * go to the tgtdev as well (refer to btrfs_map_block()).
	 */
	dev_replace->replace_state = BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED;
	dev_replace->time_started = ktime_get_real_seconds();
	dev_replace->cursor_left = 0;
	dev_replace->committed_cursor_left = 0;
	dev_replace->cursor_left_last_write_of_item = 0;
	dev_replace->cursor_right = 0;
	dev_replace->is_valid = 1;
	dev_replace->item_needs_writeback = 1;
	atomic64_set(&dev_replace->num_write_errors, 0);
	atomic64_set(&dev_replace->num_uncorrectable_read_errors, 0);
	up_write(&dev_replace->rwsem);
	need_unlock = false;

	ret = btrfs_sysfs_add_device_link(tgt_device->fs_devices, tgt_device);
	if (ret)
		btrfs_err(fs_info, "kobj add dev failed %d", ret);

	btrfs_wait_ordered_roots(fs_info, U64_MAX, 0, (u64)-1);

	/* force writing the updated state information to disk */
	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		need_unlock = true;
		down_write(&dev_replace->rwsem);
		dev_replace->replace_state =
			BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED;
		dev_replace->srcdev = NULL;
		dev_replace->tgtdev = NULL;
		goto leave;
	}

	ret = btrfs_commit_transaction(trans);
	WARN_ON(ret);

	/* the disk copy procedure reuses the scrub code */
	ret = btrfs_scrub_dev(fs_info, src_device->devid, 0,
			      btrfs_device_get_total_bytes(src_device),
			      &dev_replace->scrub_progress, 0, 1);

	ret = btrfs_dev_replace_finishing(fs_info, ret);
	if (ret == -EINPROGRESS) {
		ret = BTRFS_IOCTL_DEV_REPLACE_RESULT_SCRUB_INPROGRESS;
	} else if (ret != -ECANCELED) {
		WARN_ON(ret);
	}

	return ret;

leave:
	if (need_unlock)
		up_write(&dev_replace->rwsem);
	btrfs_destroy_dev_replace_tgtdev(tgt_device);
	return ret;
}