int btrfs_scrub_cancel(struct btrfs_fs_info *fs_info)
{
	mutex_lock(&fs_info->scrub_lock);
	if (!atomic_read(&fs_info->scrubs_running)) {
		mutex_unlock(&fs_info->scrub_lock);
		return -ENOTCONN;
	}

	atomic_inc(&fs_info->scrub_cancel_req);
	while (atomic_read(&fs_info->scrubs_running)) {
		mutex_unlock(&fs_info->scrub_lock);
		wait_event(fs_info->scrub_pause_wait,
			   atomic_read(&fs_info->scrubs_running) == 0);
		mutex_lock(&fs_info->scrub_lock);
	}
	atomic_dec(&fs_info->scrub_cancel_req);
	mutex_unlock(&fs_info->scrub_lock);

	return 0;
}