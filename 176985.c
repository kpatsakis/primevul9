static noinline void unlock_up(struct btrfs_path *path, int level,
			       int lowest_unlock, int min_write_lock_level,
			       int *write_lock_level)
{
	int i;
	int skip_level = level;
	int no_skips = 0;
	struct extent_buffer *t;

	for (i = level; i < BTRFS_MAX_LEVEL; i++) {
		if (!path->nodes[i])
			break;
		if (!path->locks[i])
			break;
		if (!no_skips && path->slots[i] == 0) {
			skip_level = i + 1;
			continue;
		}
		if (!no_skips && path->keep_locks) {
			u32 nritems;
			t = path->nodes[i];
			nritems = btrfs_header_nritems(t);
			if (nritems < 1 || path->slots[i] >= nritems - 1) {
				skip_level = i + 1;
				continue;
			}
		}
		if (skip_level < i && i >= lowest_unlock)
			no_skips = 1;

		t = path->nodes[i];
		if (i >= lowest_unlock && i > skip_level) {
			btrfs_tree_unlock_rw(t, path->locks[i]);
			path->locks[i] = 0;
			if (write_lock_level &&
			    i > min_write_lock_level &&
			    i <= *write_lock_level) {
				*write_lock_level = i - 1;
			}
		}
	}
}