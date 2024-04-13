static noinline int split_leaf(struct btrfs_trans_handle *trans,
			       struct btrfs_root *root,
			       const struct btrfs_key *ins_key,
			       struct btrfs_path *path, int data_size,
			       int extend)
{
	struct btrfs_disk_key disk_key;
	struct extent_buffer *l;
	u32 nritems;
	int mid;
	int slot;
	struct extent_buffer *right;
	struct btrfs_fs_info *fs_info = root->fs_info;
	int ret = 0;
	int wret;
	int split;
	int num_doubles = 0;
	int tried_avoid_double = 0;

	l = path->nodes[0];
	slot = path->slots[0];
	if (extend && data_size + btrfs_item_size_nr(l, slot) +
	    sizeof(struct btrfs_item) > BTRFS_LEAF_DATA_SIZE(fs_info))
		return -EOVERFLOW;

	/* first try to make some room by pushing left and right */
	if (data_size && path->nodes[1]) {
		int space_needed = data_size;

		if (slot < btrfs_header_nritems(l))
			space_needed -= btrfs_leaf_free_space(l);

		wret = push_leaf_right(trans, root, path, space_needed,
				       space_needed, 0, 0);
		if (wret < 0)
			return wret;
		if (wret) {
			space_needed = data_size;
			if (slot > 0)
				space_needed -= btrfs_leaf_free_space(l);
			wret = push_leaf_left(trans, root, path, space_needed,
					      space_needed, 0, (u32)-1);
			if (wret < 0)
				return wret;
		}
		l = path->nodes[0];

		/* did the pushes work? */
		if (btrfs_leaf_free_space(l) >= data_size)
			return 0;
	}

	if (!path->nodes[1]) {
		ret = insert_new_root(trans, root, path, 1);
		if (ret)
			return ret;
	}
again:
	split = 1;
	l = path->nodes[0];
	slot = path->slots[0];
	nritems = btrfs_header_nritems(l);
	mid = (nritems + 1) / 2;

	if (mid <= slot) {
		if (nritems == 1 ||
		    leaf_space_used(l, mid, nritems - mid) + data_size >
			BTRFS_LEAF_DATA_SIZE(fs_info)) {
			if (slot >= nritems) {
				split = 0;
			} else {
				mid = slot;
				if (mid != nritems &&
				    leaf_space_used(l, mid, nritems - mid) +
				    data_size > BTRFS_LEAF_DATA_SIZE(fs_info)) {
					if (data_size && !tried_avoid_double)
						goto push_for_double;
					split = 2;
				}
			}
		}
	} else {
		if (leaf_space_used(l, 0, mid) + data_size >
			BTRFS_LEAF_DATA_SIZE(fs_info)) {
			if (!extend && data_size && slot == 0) {
				split = 0;
			} else if ((extend || !data_size) && slot == 0) {
				mid = 1;
			} else {
				mid = slot;
				if (mid != nritems &&
				    leaf_space_used(l, mid, nritems - mid) +
				    data_size > BTRFS_LEAF_DATA_SIZE(fs_info)) {
					if (data_size && !tried_avoid_double)
						goto push_for_double;
					split = 2;
				}
			}
		}
	}

	if (split == 0)
		btrfs_cpu_key_to_disk(&disk_key, ins_key);
	else
		btrfs_item_key(l, &disk_key, mid);

	/*
	 * We have to about BTRFS_NESTING_NEW_ROOT here if we've done a double
	 * split, because we're only allowed to have MAX_LOCKDEP_SUBCLASSES
	 * subclasses, which is 8 at the time of this patch, and we've maxed it
	 * out.  In the future we could add a
	 * BTRFS_NESTING_SPLIT_THE_SPLITTENING if we need to, but for now just
	 * use BTRFS_NESTING_NEW_ROOT.
	 */
	right = alloc_tree_block_no_bg_flush(trans, root, 0, &disk_key, 0,
					     l->start, 0, num_doubles ?
					     BTRFS_NESTING_NEW_ROOT :
					     BTRFS_NESTING_SPLIT);
	if (IS_ERR(right))
		return PTR_ERR(right);

	root_add_used(root, fs_info->nodesize);

	if (split == 0) {
		if (mid <= slot) {
			btrfs_set_header_nritems(right, 0);
			insert_ptr(trans, path, &disk_key,
				   right->start, path->slots[1] + 1, 1);
			btrfs_tree_unlock(path->nodes[0]);
			free_extent_buffer(path->nodes[0]);
			path->nodes[0] = right;
			path->slots[0] = 0;
			path->slots[1] += 1;
		} else {
			btrfs_set_header_nritems(right, 0);
			insert_ptr(trans, path, &disk_key,
				   right->start, path->slots[1], 1);
			btrfs_tree_unlock(path->nodes[0]);
			free_extent_buffer(path->nodes[0]);
			path->nodes[0] = right;
			path->slots[0] = 0;
			if (path->slots[1] == 0)
				fixup_low_keys(path, &disk_key, 1);
		}
		/*
		 * We create a new leaf 'right' for the required ins_len and
		 * we'll do btrfs_mark_buffer_dirty() on this leaf after copying
		 * the content of ins_len to 'right'.
		 */
		return ret;
	}

	copy_for_split(trans, path, l, right, slot, mid, nritems);

	if (split == 2) {
		BUG_ON(num_doubles != 0);
		num_doubles++;
		goto again;
	}

	return 0;

push_for_double:
	push_for_double_split(trans, root, path, data_size);
	tried_avoid_double = 1;
	if (btrfs_leaf_free_space(path->nodes[0]) >= data_size)
		return 0;
	goto again;
}