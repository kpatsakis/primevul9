static inline int tree_mod_need_log(const struct btrfs_fs_info *fs_info,
				    struct extent_buffer *eb)
{
	smp_mb();
	if (list_empty(&(fs_info)->tree_mod_seq_list))
		return 0;
	if (eb && btrfs_header_level(eb) == 0)
		return 0;

	return 1;
}