int btrfs_resume_dev_replace_async(struct btrfs_fs_info *fs_info)
{
	struct task_struct *task;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;

	down_write(&dev_replace->rwsem);

	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
		up_write(&dev_replace->rwsem);
		return 0;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		dev_replace->replace_state =
			BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED;
		break;
	}
	if (!dev_replace->tgtdev || !dev_replace->tgtdev->bdev) {
		btrfs_info(fs_info,
			   "cannot continue dev_replace, tgtdev is missing");
		btrfs_info(fs_info,
			   "you may cancel the operation after 'mount -o degraded'");
		dev_replace->replace_state =
					BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED;
		up_write(&dev_replace->rwsem);
		return 0;
	}
	up_write(&dev_replace->rwsem);

	/*
	 * This could collide with a paused balance, but the exclusive op logic
	 * should never allow both to start and pause. We don't want to allow
	 * dev-replace to start anyway.
	 */
	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags)) {
		down_write(&dev_replace->rwsem);
		dev_replace->replace_state =
					BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED;
		up_write(&dev_replace->rwsem);
		btrfs_info(fs_info,
		"cannot resume dev-replace, other exclusive operation running");
		return 0;
	}

	task = kthread_run(btrfs_dev_replace_kthread, fs_info, "btrfs-devrepl");
	return PTR_ERR_OR_ZERO(task);
}