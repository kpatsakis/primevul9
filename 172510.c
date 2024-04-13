int btrfs_dev_replace_cancel(struct btrfs_fs_info *fs_info)
{
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	struct btrfs_device *tgt_device = NULL;
	struct btrfs_device *src_device = NULL;
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = fs_info->tree_root;
	int result;
	int ret;

	if (sb_rdonly(fs_info->sb))
		return -EROFS;

	mutex_lock(&dev_replace->lock_finishing_cancel_unmount);
	down_write(&dev_replace->rwsem);
	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
		result = BTRFS_IOCTL_DEV_REPLACE_RESULT_NOT_STARTED;
		up_write(&dev_replace->rwsem);
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
		tgt_device = dev_replace->tgtdev;
		src_device = dev_replace->srcdev;
		up_write(&dev_replace->rwsem);
		ret = btrfs_scrub_cancel(fs_info);
		if (ret < 0) {
			result = BTRFS_IOCTL_DEV_REPLACE_RESULT_NOT_STARTED;
		} else {
			result = BTRFS_IOCTL_DEV_REPLACE_RESULT_NO_ERROR;
			/*
			 * btrfs_dev_replace_finishing() will handle the
			 * cleanup part
			 */
			btrfs_info_in_rcu(fs_info,
				"dev_replace from %s (devid %llu) to %s canceled",
				btrfs_dev_name(src_device), src_device->devid,
				btrfs_dev_name(tgt_device));
		}
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		/*
		 * Scrub doing the replace isn't running so we need to do the
		 * cleanup step of btrfs_dev_replace_finishing() here
		 */
		result = BTRFS_IOCTL_DEV_REPLACE_RESULT_NO_ERROR;
		tgt_device = dev_replace->tgtdev;
		src_device = dev_replace->srcdev;
		dev_replace->tgtdev = NULL;
		dev_replace->srcdev = NULL;
		dev_replace->replace_state =
				BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED;
		dev_replace->time_stopped = ktime_get_real_seconds();
		dev_replace->item_needs_writeback = 1;

		up_write(&dev_replace->rwsem);

		/* Scrub for replace must not be running in suspended state */
		ret = btrfs_scrub_cancel(fs_info);
		ASSERT(ret != -ENOTCONN);

		trans = btrfs_start_transaction(root, 0);
		if (IS_ERR(trans)) {
			mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
			return PTR_ERR(trans);
		}
		ret = btrfs_commit_transaction(trans);
		WARN_ON(ret);

		btrfs_info_in_rcu(fs_info,
		"suspended dev_replace from %s (devid %llu) to %s canceled",
			btrfs_dev_name(src_device), src_device->devid,
			btrfs_dev_name(tgt_device));

		if (tgt_device)
			btrfs_destroy_dev_replace_tgtdev(tgt_device);
		break;
	default:
		result = -EINVAL;
	}

	mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
	return result;
}