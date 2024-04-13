setup_nodes_for_search(struct btrfs_trans_handle *trans,
		       struct btrfs_root *root, struct btrfs_path *p,
		       struct extent_buffer *b, int level, int ins_len,
		       int *write_lock_level)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	int ret = 0;

	if ((p->search_for_split || ins_len > 0) && btrfs_header_nritems(b) >=
	    BTRFS_NODEPTRS_PER_BLOCK(fs_info) - 3) {

		if (*write_lock_level < level + 1) {
			*write_lock_level = level + 1;
			btrfs_release_path(p);
			return -EAGAIN;
		}

		reada_for_balance(p, level);
		ret = split_node(trans, root, p, level);

		b = p->nodes[level];
	} else if (ins_len < 0 && btrfs_header_nritems(b) <
		   BTRFS_NODEPTRS_PER_BLOCK(fs_info) / 2) {

		if (*write_lock_level < level + 1) {
			*write_lock_level = level + 1;
			btrfs_release_path(p);
			return -EAGAIN;
		}

		reada_for_balance(p, level);
		ret = balance_level(trans, root, p, level);
		if (ret)
			return ret;

		b = p->nodes[level];
		if (!b) {
			btrfs_release_path(p);
			return -EAGAIN;
		}
		BUG_ON(btrfs_header_nritems(b) == 1);
	}
	return ret;
}