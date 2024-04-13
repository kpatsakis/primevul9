void btrfs_bio_counter_inc_blocked(struct btrfs_fs_info *fs_info)
{
	while (1) {
		percpu_counter_inc(&fs_info->dev_replace.bio_counter);
		if (likely(!test_bit(BTRFS_FS_STATE_DEV_REPLACING,
				     &fs_info->fs_state)))
			break;

		btrfs_bio_counter_dec(fs_info);
		wait_event(fs_info->dev_replace.replace_wait,
			   !test_bit(BTRFS_FS_STATE_DEV_REPLACING,
				     &fs_info->fs_state));
	}
}