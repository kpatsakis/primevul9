static noinline int split_node(struct btrfs_trans_handle *trans,
			       struct btrfs_root *root,
			       struct btrfs_path *path, int level)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *c;
	struct extent_buffer *split;
	struct btrfs_disk_key disk_key;
	int mid;
	int ret;
	u32 c_nritems;

	c = path->nodes[level];
	WARN_ON(btrfs_header_generation(c) != trans->transid);
	if (c == root->node) {
		/*
		 * trying to split the root, lets make a new one
		 *
		 * tree mod log: We don't log_removal old root in
		 * insert_new_root, because that root buffer will be kept as a
		 * normal node. We are going to log removal of half of the
		 * elements below with tree_mod_log_eb_copy. We're holding a
		 * tree lock on the buffer, which is why we cannot race with
		 * other tree_mod_log users.
		 */
		ret = insert_new_root(trans, root, path, level + 1);
		if (ret)
			return ret;
	} else {
		ret = push_nodes_for_insert(trans, root, path, level);
		c = path->nodes[level];
		if (!ret && btrfs_header_nritems(c) <
		    BTRFS_NODEPTRS_PER_BLOCK(fs_info) - 3)
			return 0;
		if (ret < 0)
			return ret;
	}

	c_nritems = btrfs_header_nritems(c);
	mid = (c_nritems + 1) / 2;
	btrfs_node_key(c, &disk_key, mid);

	split = alloc_tree_block_no_bg_flush(trans, root, 0, &disk_key, level,
					     c->start, 0, BTRFS_NESTING_SPLIT);
	if (IS_ERR(split))
		return PTR_ERR(split);

	root_add_used(root, fs_info->nodesize);
	ASSERT(btrfs_header_level(c) == level);

	ret = tree_mod_log_eb_copy(split, c, 0, mid, c_nritems - mid);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		return ret;
	}
	copy_extent_buffer(split, c,
			   btrfs_node_key_ptr_offset(0),
			   btrfs_node_key_ptr_offset(mid),
			   (c_nritems - mid) * sizeof(struct btrfs_key_ptr));
	btrfs_set_header_nritems(split, c_nritems - mid);
	btrfs_set_header_nritems(c, mid);

	btrfs_mark_buffer_dirty(c);
	btrfs_mark_buffer_dirty(split);

	insert_ptr(trans, path, &disk_key, split->start,
		   path->slots[level + 1] + 1, level + 1);

	if (path->slots[level] >= mid) {
		path->slots[level] -= mid;
		btrfs_tree_unlock(c);
		free_extent_buffer(c);
		path->nodes[level] = split;
		path->slots[level + 1] += 1;
	} else {
		btrfs_tree_unlock(split);
		free_extent_buffer(split);
	}
	return 0;
}