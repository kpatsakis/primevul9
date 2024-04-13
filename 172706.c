static u64 btrfs_dev_replace_progress(struct btrfs_fs_info *fs_info)
{
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	u64 ret = 0;

	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
		ret = 0;
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
		ret = 1000;
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		ret = div64_u64(dev_replace->cursor_left,
				div_u64(btrfs_device_get_total_bytes(
						dev_replace->srcdev), 1000));
		break;
	}

	return ret;
}