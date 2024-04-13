static void scrub_pause_off(struct btrfs_fs_info *fs_info)
{
	mutex_lock(&fs_info->scrub_lock);
	__scrub_blocked_if_needed(fs_info);
	atomic_dec(&fs_info->scrubs_paused);
	mutex_unlock(&fs_info->scrub_lock);

	wake_up(&fs_info->scrub_pause_wait);
}