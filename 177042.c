static noinline int push_for_double_split(struct btrfs_trans_handle *trans,
					  struct btrfs_root *root,
					  struct btrfs_path *path,
					  int data_size)
{
	int ret;
	int progress = 0;
	int slot;
	u32 nritems;
	int space_needed = data_size;

	slot = path->slots[0];
	if (slot < btrfs_header_nritems(path->nodes[0]))
		space_needed -= btrfs_leaf_free_space(path->nodes[0]);

	/*
	 * try to push all the items after our slot into the
	 * right leaf
	 */
	ret = push_leaf_right(trans, root, path, 1, space_needed, 0, slot);
	if (ret < 0)
		return ret;

	if (ret == 0)
		progress++;

	nritems = btrfs_header_nritems(path->nodes[0]);
	/*
	 * our goal is to get our slot at the start or end of a leaf.  If
	 * we've done so we're done
	 */
	if (path->slots[0] == 0 || path->slots[0] == nritems)
		return 0;

	if (btrfs_leaf_free_space(path->nodes[0]) >= data_size)
		return 0;

	/* try to push all the items before our slot into the next leaf */
	slot = path->slots[0];
	space_needed = data_size;
	if (slot > 0)
		space_needed -= btrfs_leaf_free_space(path->nodes[0]);
	ret = push_leaf_left(trans, root, path, 1, space_needed, 0, slot);
	if (ret < 0)
		return ret;

	if (ret == 0)
		progress++;

	if (progress)
		return 0;
	return 1;
}