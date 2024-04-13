static inline int tree_mod_dont_log(struct btrfs_fs_info *fs_info,
				    struct extent_buffer *eb) {
	smp_mb();
	if (list_empty(&(fs_info)->tree_mod_seq_list))
		return 1;
	if (eb && btrfs_header_level(eb) == 0)
		return 1;

	write_lock(&fs_info->tree_mod_log_lock);
	if (list_empty(&(fs_info)->tree_mod_seq_list)) {
		write_unlock(&fs_info->tree_mod_log_lock);
		return 1;
	}

	return 0;
}