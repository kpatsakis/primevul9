static void btrfs_rm_dev_replace_blocked(struct btrfs_fs_info *fs_info)
{
	set_bit(BTRFS_FS_STATE_DEV_REPLACING, &fs_info->fs_state);
	wait_event(fs_info->dev_replace.replace_wait, !percpu_counter_sum(
		   &fs_info->dev_replace.bio_counter));
}