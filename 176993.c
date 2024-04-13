int btrfs_next_old_leaf(struct btrfs_root *root, struct btrfs_path *path,
			u64 time_seq)
{
	int slot;
	int level;
	struct extent_buffer *c;
	struct extent_buffer *next;
	struct btrfs_key key;
	u32 nritems;
	int ret;
	int i;

	nritems = btrfs_header_nritems(path->nodes[0]);
	if (nritems == 0)
		return 1;

	btrfs_item_key_to_cpu(path->nodes[0], &key, nritems - 1);
again:
	level = 1;
	next = NULL;
	btrfs_release_path(path);

	path->keep_locks = 1;

	if (time_seq)
		ret = btrfs_search_old_slot(root, &key, path, time_seq);
	else
		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	path->keep_locks = 0;

	if (ret < 0)
		return ret;

	nritems = btrfs_header_nritems(path->nodes[0]);
	/*
	 * by releasing the path above we dropped all our locks.  A balance
	 * could have added more items next to the key that used to be
	 * at the very end of the block.  So, check again here and
	 * advance the path if there are now more items available.
	 */
	if (nritems > 0 && path->slots[0] < nritems - 1) {
		if (ret == 0)
			path->slots[0]++;
		ret = 0;
		goto done;
	}
	/*
	 * So the above check misses one case:
	 * - after releasing the path above, someone has removed the item that
	 *   used to be at the very end of the block, and balance between leafs
	 *   gets another one with bigger key.offset to replace it.
	 *
	 * This one should be returned as well, or we can get leaf corruption
	 * later(esp. in __btrfs_drop_extents()).
	 *
	 * And a bit more explanation about this check,
	 * with ret > 0, the key isn't found, the path points to the slot
	 * where it should be inserted, so the path->slots[0] item must be the
	 * bigger one.
	 */
	if (nritems > 0 && ret > 0 && path->slots[0] == nritems - 1) {
		ret = 0;
		goto done;
	}

	while (level < BTRFS_MAX_LEVEL) {
		if (!path->nodes[level]) {
			ret = 1;
			goto done;
		}

		slot = path->slots[level] + 1;
		c = path->nodes[level];
		if (slot >= btrfs_header_nritems(c)) {
			level++;
			if (level == BTRFS_MAX_LEVEL) {
				ret = 1;
				goto done;
			}
			continue;
		}


		/*
		 * Our current level is where we're going to start from, and to
		 * make sure lockdep doesn't complain we need to drop our locks
		 * and nodes from 0 to our current level.
		 */
		for (i = 0; i < level; i++) {
			if (path->locks[level]) {
				btrfs_tree_read_unlock(path->nodes[i]);
				path->locks[i] = 0;
			}
			free_extent_buffer(path->nodes[i]);
			path->nodes[i] = NULL;
		}

		next = c;
		ret = read_block_for_search(root, path, &next, level,
					    slot, &key);
		if (ret == -EAGAIN)
			goto again;

		if (ret < 0) {
			btrfs_release_path(path);
			goto done;
		}

		if (!path->skip_locking) {
			ret = btrfs_try_tree_read_lock(next);
			if (!ret && time_seq) {
				/*
				 * If we don't get the lock, we may be racing
				 * with push_leaf_left, holding that lock while
				 * itself waiting for the leaf we've currently
				 * locked. To solve this situation, we give up
				 * on our lock and cycle.
				 */
				free_extent_buffer(next);
				btrfs_release_path(path);
				cond_resched();
				goto again;
			}
			if (!ret)
				btrfs_tree_read_lock(next);
		}
		break;
	}
	path->slots[level] = slot;
	while (1) {
		level--;
		path->nodes[level] = next;
		path->slots[level] = 0;
		if (!path->skip_locking)
			path->locks[level] = BTRFS_READ_LOCK;
		if (!level)
			break;

		ret = read_block_for_search(root, path, &next, level,
					    0, &key);
		if (ret == -EAGAIN)
			goto again;

		if (ret < 0) {
			btrfs_release_path(path);
			goto done;
		}

		if (!path->skip_locking)
			btrfs_tree_read_lock(next);
	}
	ret = 0;
done:
	unlock_up(path, 0, 1, 0, NULL);

	return ret;
}