void btrfs_dev_replace_suspend_for_unmount(struct btrfs_fs_info *fs_info)
{
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;

	mutex_lock(&dev_replace->lock_finishing_cancel_unmount);
	down_write(&dev_replace->rwsem);

	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
		dev_replace->replace_state =
			BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED;
		dev_replace->time_stopped = ktime_get_real_seconds();
		dev_replace->item_needs_writeback = 1;
		btrfs_info(fs_info, "suspending dev_replace for unmount");
		break;
	}

	up_write(&dev_replace->rwsem);
	mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
}