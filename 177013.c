u64 btrfs_get_tree_mod_seq(struct btrfs_fs_info *fs_info,
			   struct seq_list *elem)
{
	write_lock(&fs_info->tree_mod_log_lock);
	if (!elem->seq) {
		elem->seq = btrfs_inc_tree_mod_seq(fs_info);
		list_add_tail(&elem->list, &fs_info->tree_mod_seq_list);
	}
	write_unlock(&fs_info->tree_mod_log_lock);

	return elem->seq;
}