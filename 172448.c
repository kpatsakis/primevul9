static void scrub_blocked_if_needed(struct btrfs_fs_info *fs_info)
{
	scrub_pause_on(fs_info);
	scrub_pause_off(fs_info);
}