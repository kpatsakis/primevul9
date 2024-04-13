void btrfs_scrub_continue(struct btrfs_fs_info *fs_info)
{
	atomic_dec(&fs_info->scrub_pause_req);
	wake_up(&fs_info->scrub_pause_wait);
}