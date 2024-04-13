void btrfs_put_tree_mod_seq(struct btrfs_fs_info *fs_info,
			    struct seq_list *elem)
{
	struct rb_root *tm_root;
	struct rb_node *node;
	struct rb_node *next;
	struct tree_mod_elem *tm;
	u64 min_seq = (u64)-1;
	u64 seq_putting = elem->seq;

	if (!seq_putting)
		return;

	write_lock(&fs_info->tree_mod_log_lock);
	list_del(&elem->list);
	elem->seq = 0;

	if (!list_empty(&fs_info->tree_mod_seq_list)) {
		struct seq_list *first;

		first = list_first_entry(&fs_info->tree_mod_seq_list,
					 struct seq_list, list);
		if (seq_putting > first->seq) {
			/*
			 * Blocker with lower sequence number exists, we
			 * cannot remove anything from the log.
			 */
			write_unlock(&fs_info->tree_mod_log_lock);
			return;
		}
		min_seq = first->seq;
	}

	/*
	 * anything that's lower than the lowest existing (read: blocked)
	 * sequence number can be removed from the tree.
	 */
	tm_root = &fs_info->tree_mod_log;
	for (node = rb_first(tm_root); node; node = next) {
		next = rb_next(node);
		tm = rb_entry(node, struct tree_mod_elem, node);
		if (tm->seq >= min_seq)
			continue;
		rb_erase(node, tm_root);
		kfree(tm);
	}
	write_unlock(&fs_info->tree_mod_log_lock);
}