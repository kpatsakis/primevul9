static noinline int balance_level(struct btrfs_trans_handle *trans,
			 struct btrfs_root *root,
			 struct btrfs_path *path, int level)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *right = NULL;
	struct extent_buffer *mid;
	struct extent_buffer *left = NULL;
	struct extent_buffer *parent = NULL;
	int ret = 0;
	int wret;
	int pslot;
	int orig_slot = path->slots[level];
	u64 orig_ptr;

	ASSERT(level > 0);

	mid = path->nodes[level];

	WARN_ON(path->locks[level] != BTRFS_WRITE_LOCK);
	WARN_ON(btrfs_header_generation(mid) != trans->transid);

	orig_ptr = btrfs_node_blockptr(mid, orig_slot);

	if (level < BTRFS_MAX_LEVEL - 1) {
		parent = path->nodes[level + 1];
		pslot = path->slots[level + 1];
	}

	/*
	 * deal with the case where there is only one pointer in the root
	 * by promoting the node below to a root
	 */
	if (!parent) {
		struct extent_buffer *child;

		if (btrfs_header_nritems(mid) != 1)
			return 0;

		/* promote the child to a root */
		child = btrfs_read_node_slot(mid, 0);
		if (IS_ERR(child)) {
			ret = PTR_ERR(child);
			btrfs_handle_fs_error(fs_info, ret, NULL);
			goto enospc;
		}

		btrfs_tree_lock(child);
		ret = btrfs_cow_block(trans, root, child, mid, 0, &child,
				      BTRFS_NESTING_COW);
		if (ret) {
			btrfs_tree_unlock(child);
			free_extent_buffer(child);
			goto enospc;
		}

		ret = tree_mod_log_insert_root(root->node, child, 1);
		BUG_ON(ret < 0);
		rcu_assign_pointer(root->node, child);

		add_root_to_dirty_list(root);
		btrfs_tree_unlock(child);

		path->locks[level] = 0;
		path->nodes[level] = NULL;
		btrfs_clean_tree_block(mid);
		btrfs_tree_unlock(mid);
		/* once for the path */
		free_extent_buffer(mid);

		root_sub_used(root, mid->len);
		btrfs_free_tree_block(trans, root, mid, 0, 1);
		/* once for the root ptr */
		free_extent_buffer_stale(mid);
		return 0;
	}
	if (btrfs_header_nritems(mid) >
	    BTRFS_NODEPTRS_PER_BLOCK(fs_info) / 4)
		return 0;

	left = btrfs_read_node_slot(parent, pslot - 1);
	if (IS_ERR(left))
		left = NULL;

	if (left) {
		__btrfs_tree_lock(left, BTRFS_NESTING_LEFT);
		wret = btrfs_cow_block(trans, root, left,
				       parent, pslot - 1, &left,
				       BTRFS_NESTING_LEFT_COW);
		if (wret) {
			ret = wret;
			goto enospc;
		}
	}

	right = btrfs_read_node_slot(parent, pslot + 1);
	if (IS_ERR(right))
		right = NULL;

	if (right) {
		__btrfs_tree_lock(right, BTRFS_NESTING_RIGHT);
		wret = btrfs_cow_block(trans, root, right,
				       parent, pslot + 1, &right,
				       BTRFS_NESTING_RIGHT_COW);
		if (wret) {
			ret = wret;
			goto enospc;
		}
	}

	/* first, try to make some room in the middle buffer */
	if (left) {
		orig_slot += btrfs_header_nritems(left);
		wret = push_node_left(trans, left, mid, 1);
		if (wret < 0)
			ret = wret;
	}

	/*
	 * then try to empty the right most buffer into the middle
	 */
	if (right) {
		wret = push_node_left(trans, mid, right, 1);
		if (wret < 0 && wret != -ENOSPC)
			ret = wret;
		if (btrfs_header_nritems(right) == 0) {
			btrfs_clean_tree_block(right);
			btrfs_tree_unlock(right);
			del_ptr(root, path, level + 1, pslot + 1);
			root_sub_used(root, right->len);
			btrfs_free_tree_block(trans, root, right, 0, 1);
			free_extent_buffer_stale(right);
			right = NULL;
		} else {
			struct btrfs_disk_key right_key;
			btrfs_node_key(right, &right_key, 0);
			ret = tree_mod_log_insert_key(parent, pslot + 1,
					MOD_LOG_KEY_REPLACE, GFP_NOFS);
			BUG_ON(ret < 0);
			btrfs_set_node_key(parent, &right_key, pslot + 1);
			btrfs_mark_buffer_dirty(parent);
		}
	}
	if (btrfs_header_nritems(mid) == 1) {
		/*
		 * we're not allowed to leave a node with one item in the
		 * tree during a delete.  A deletion from lower in the tree
		 * could try to delete the only pointer in this node.
		 * So, pull some keys from the left.
		 * There has to be a left pointer at this point because
		 * otherwise we would have pulled some pointers from the
		 * right
		 */
		if (!left) {
			ret = -EROFS;
			btrfs_handle_fs_error(fs_info, ret, NULL);
			goto enospc;
		}
		wret = balance_node_right(trans, mid, left);
		if (wret < 0) {
			ret = wret;
			goto enospc;
		}
		if (wret == 1) {
			wret = push_node_left(trans, left, mid, 1);
			if (wret < 0)
				ret = wret;
		}
		BUG_ON(wret == 1);
	}
	if (btrfs_header_nritems(mid) == 0) {
		btrfs_clean_tree_block(mid);
		btrfs_tree_unlock(mid);
		del_ptr(root, path, level + 1, pslot);
		root_sub_used(root, mid->len);
		btrfs_free_tree_block(trans, root, mid, 0, 1);
		free_extent_buffer_stale(mid);
		mid = NULL;
	} else {
		/* update the parent key to reflect our changes */
		struct btrfs_disk_key mid_key;
		btrfs_node_key(mid, &mid_key, 0);
		ret = tree_mod_log_insert_key(parent, pslot,
				MOD_LOG_KEY_REPLACE, GFP_NOFS);
		BUG_ON(ret < 0);
		btrfs_set_node_key(parent, &mid_key, pslot);
		btrfs_mark_buffer_dirty(parent);
	}

	/* update the path */
	if (left) {
		if (btrfs_header_nritems(left) > orig_slot) {
			atomic_inc(&left->refs);
			/* left was locked after cow */
			path->nodes[level] = left;
			path->slots[level + 1] -= 1;
			path->slots[level] = orig_slot;
			if (mid) {
				btrfs_tree_unlock(mid);
				free_extent_buffer(mid);
			}
		} else {
			orig_slot -= btrfs_header_nritems(left);
			path->slots[level] = orig_slot;
		}
	}
	/* double check we haven't messed things up */
	if (orig_ptr !=
	    btrfs_node_blockptr(path->nodes[level], path->slots[level]))
		BUG();
enospc:
	if (right) {
		btrfs_tree_unlock(right);
		free_extent_buffer(right);
	}
	if (left) {
		if (path->nodes[level] != left)
			btrfs_tree_unlock(left);
		free_extent_buffer(left);
	}
	return ret;
}