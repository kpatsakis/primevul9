static int push_leaf_right(struct btrfs_trans_handle *trans, struct btrfs_root
			   *root, struct btrfs_path *path,
			   int min_data_size, int data_size,
			   int empty, u32 min_slot)
{
	struct extent_buffer *left = path->nodes[0];
	struct extent_buffer *right;
	struct extent_buffer *upper;
	int slot;
	int free_space;
	u32 left_nritems;
	int ret;

	if (!path->nodes[1])
		return 1;

	slot = path->slots[1];
	upper = path->nodes[1];
	if (slot >= btrfs_header_nritems(upper) - 1)
		return 1;

	btrfs_assert_tree_locked(path->nodes[1]);

	right = btrfs_read_node_slot(upper, slot + 1);
	/*
	 * slot + 1 is not valid or we fail to read the right node,
	 * no big deal, just return.
	 */
	if (IS_ERR(right))
		return 1;

	__btrfs_tree_lock(right, BTRFS_NESTING_RIGHT);

	free_space = btrfs_leaf_free_space(right);
	if (free_space < data_size)
		goto out_unlock;

	/* cow and double check */
	ret = btrfs_cow_block(trans, root, right, upper,
			      slot + 1, &right, BTRFS_NESTING_RIGHT_COW);
	if (ret)
		goto out_unlock;

	free_space = btrfs_leaf_free_space(right);
	if (free_space < data_size)
		goto out_unlock;

	left_nritems = btrfs_header_nritems(left);
	if (left_nritems == 0)
		goto out_unlock;

	if (check_sibling_keys(left, right)) {
		ret = -EUCLEAN;
		btrfs_tree_unlock(right);
		free_extent_buffer(right);
		return ret;
	}
	if (path->slots[0] == left_nritems && !empty) {
		/* Key greater than all keys in the leaf, right neighbor has
		 * enough room for it and we're not emptying our leaf to delete
		 * it, therefore use right neighbor to insert the new item and
		 * no need to touch/dirty our left leaf. */
		btrfs_tree_unlock(left);
		free_extent_buffer(left);
		path->nodes[0] = right;
		path->slots[0] = 0;
		path->slots[1]++;
		return 0;
	}

	return __push_leaf_right(path, min_data_size, empty,
				right, free_space, left_nritems, min_slot);
out_unlock:
	btrfs_tree_unlock(right);
	free_extent_buffer(right);
	return 1;
}