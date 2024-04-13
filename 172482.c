void btrfs_bio_counter_sub(struct btrfs_fs_info *fs_info, s64 amount)
{
	percpu_counter_sub(&fs_info->dev_replace.bio_counter, amount);
	cond_wake_up_nomb(&fs_info->dev_replace.replace_wait);
}