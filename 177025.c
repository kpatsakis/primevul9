tree_mod_log_search_oldest(struct btrfs_fs_info *fs_info, u64 start,
			   u64 min_seq)
{
	return __tree_mod_log_search(fs_info, start, min_seq, 1);
}