void btrfs_bio_counter_inc_noblocked(struct btrfs_fs_info *fs_info)
{
	percpu_counter_inc(&fs_info->dev_replace.bio_counter);
}