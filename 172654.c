void btrfs_dev_replace_status(struct btrfs_fs_info *fs_info,
			      struct btrfs_ioctl_dev_replace_args *args)
{
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;

	down_read(&dev_replace->rwsem);
	/* even if !dev_replace_is_valid, the values are good enough for
	 * the replace_status ioctl */
	args->result = BTRFS_IOCTL_DEV_REPLACE_RESULT_NO_ERROR;
	args->status.replace_state = dev_replace->replace_state;
	args->status.time_started = dev_replace->time_started;
	args->status.time_stopped = dev_replace->time_stopped;
	args->status.num_write_errors =
		atomic64_read(&dev_replace->num_write_errors);
	args->status.num_uncorrectable_read_errors =
		atomic64_read(&dev_replace->num_uncorrectable_read_errors);
	args->status.progress_1000 = btrfs_dev_replace_progress(fs_info);
	up_read(&dev_replace->rwsem);
}