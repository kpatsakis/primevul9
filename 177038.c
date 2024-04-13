static inline u64 btrfs_inc_tree_mod_seq(struct btrfs_fs_info *fs_info)
{
	return atomic64_inc_return(&fs_info->tree_mod_seq);
}