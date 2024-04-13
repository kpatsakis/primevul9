int btrfs_search_old_slot(struct btrfs_root *root, const struct btrfs_key *key,
			  struct btrfs_path *p, u64 time_seq)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *b;
	int slot;
	int ret;
	int err;
	int level;
	int lowest_unlock = 1;
	u8 lowest_level = 0;

	lowest_level = p->lowest_level;
	WARN_ON(p->nodes[0] != NULL);

	if (p->search_commit_root) {
		BUG_ON(time_seq);
		return btrfs_search_slot(NULL, root, key, p, 0, 0);
	}

again:
	b = get_old_root(root, time_seq);
	if (!b) {
		ret = -EIO;
		goto done;
	}
	level = btrfs_header_level(b);
	p->locks[level] = BTRFS_READ_LOCK;

	while (b) {
		int dec = 0;

		level = btrfs_header_level(b);
		p->nodes[level] = b;

		/*
		 * we have a lock on b and as long as we aren't changing
		 * the tree, there is no way to for the items in b to change.
		 * It is safe to drop the lock on our parent before we
		 * go through the expensive btree search on b.
		 */
		btrfs_unlock_up_safe(p, level + 1);

		ret = btrfs_bin_search(b, key, &slot);
		if (ret < 0)
			goto done;

		if (level == 0) {
			p->slots[level] = slot;
			unlock_up(p, level, lowest_unlock, 0, NULL);
			goto done;
		}

		if (ret && slot > 0) {
			dec = 1;
			slot--;
		}
		p->slots[level] = slot;
		unlock_up(p, level, lowest_unlock, 0, NULL);

		if (level == lowest_level) {
			if (dec)
				p->slots[level]++;
			goto done;
		}

		err = read_block_for_search(root, p, &b, level, slot, key);
		if (err == -EAGAIN)
			goto again;
		if (err) {
			ret = err;
			goto done;
		}

		level = btrfs_header_level(b);
		btrfs_tree_read_lock(b);
		b = tree_mod_log_rewind(fs_info, p, b, time_seq);
		if (!b) {
			ret = -ENOMEM;
			goto done;
		}
		p->locks[level] = BTRFS_READ_LOCK;
		p->nodes[level] = b;
	}
	ret = 1;
done:
	if (ret < 0)
		btrfs_release_path(p);

	return ret;
}