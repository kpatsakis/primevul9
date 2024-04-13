static int push_leaf_left(struct btrfs_trans_handle *trans, struct btrfs_root
			  *root, struct btrfs_path *path, int min_data_size,
			  int data_size, int empty, u32 max_slot)
{
	struct extent_buffer *right = path->nodes[0];
	struct extent_buffer *left;
	int slot;
	int free_space;
	u32 right_nritems;
	int ret = 0;

	slot = path->slots[1];
	if (slot == 0)
		return 1;
	if (!path->nodes[1])
		return 1;

	right_nritems = btrfs_header_nritems(right);
	if (right_nritems == 0)
		return 1;

	btrfs_assert_tree_locked(path->nodes[1]);

	left = btrfs_read_node_slot(path->nodes[1], slot - 1);
	/*
	 * slot - 1 is not valid or we fail to read the left node,
	 * no big deal, just return.
	 */
	if (IS_ERR(left))
		return 1;

	__btrfs_tree_lock(left, BTRFS_NESTING_LEFT);

	free_space = btrfs_leaf_free_space(left);
	if (free_space < data_size) {
		ret = 1;
		goto out;
	}

	/* cow and double check */
	ret = btrfs_cow_block(trans, root, left,
			      path->nodes[1], slot - 1, &left,
			      BTRFS_NESTING_LEFT_COW);
	if (ret) {
		/* we hit -ENOSPC, but it isn't fatal here */
		if (ret == -ENOSPC)
			ret = 1;
		goto out;
	}

	free_space = btrfs_leaf_free_space(left);
	if (free_space < data_size) {
		ret = 1;
		goto out;
	}

	if (check_sibling_keys(left, right)) {
		ret = -EUCLEAN;
		goto out;
	}
	return __push_leaf_left(path, min_data_size,
			       empty, left, free_space, right_nritems,
			       max_slot);
out:
	btrfs_tree_unlock(left);
	free_extent_buffer(left);
	return ret;
}