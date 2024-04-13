__tree_mod_log_search(struct btrfs_fs_info *fs_info, u64 start, u64 min_seq,
		      int smallest)
{
	struct rb_root *tm_root;
	struct rb_node *node;
	struct tree_mod_elem *cur = NULL;
	struct tree_mod_elem *found = NULL;

	read_lock(&fs_info->tree_mod_log_lock);
	tm_root = &fs_info->tree_mod_log;
	node = tm_root->rb_node;
	while (node) {
		cur = rb_entry(node, struct tree_mod_elem, node);
		if (cur->logical < start) {
			node = node->rb_left;
		} else if (cur->logical > start) {
			node = node->rb_right;
		} else if (cur->seq < min_seq) {
			node = node->rb_left;
		} else if (!smallest) {
			/* we want the node with the highest seq */
			if (found)
				BUG_ON(found->seq > cur->seq);
			found = cur;
			node = node->rb_left;
		} else if (cur->seq > min_seq) {
			/* we want the node with the smallest seq */
			if (found)
				BUG_ON(found->seq < cur->seq);
			found = cur;
			node = node->rb_right;
		} else {
			found = cur;
			break;
		}
	}
	read_unlock(&fs_info->tree_mod_log_lock);

	return found;
}