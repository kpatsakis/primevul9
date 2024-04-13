static int __btrfs_balance(struct btrfs_fs_info *fs_info)
{
	struct btrfs_balance_control *bctl = fs_info->balance_ctl;
	struct btrfs_root *chunk_root = fs_info->chunk_root;
	u64 chunk_type;
	struct btrfs_chunk *chunk;
	struct btrfs_path *path = NULL;
	struct btrfs_key key;
	struct btrfs_key found_key;
	struct extent_buffer *leaf;
	int slot;
	int ret;
	int enospc_errors = 0;
	bool counting = true;
	/* The single value limit and min/max limits use the same bytes in the */
	u64 limit_data = bctl->data.limit;
	u64 limit_meta = bctl->meta.limit;
	u64 limit_sys = bctl->sys.limit;
	u32 count_data = 0;
	u32 count_meta = 0;
	u32 count_sys = 0;
	int chunk_reserved = 0;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto error;
	}

	/* zero out stat counters */
	spin_lock(&fs_info->balance_lock);
	memset(&bctl->stat, 0, sizeof(bctl->stat));
	spin_unlock(&fs_info->balance_lock);
again:
	if (!counting) {
		/*
		 * The single value limit and min/max limits use the same bytes
		 * in the
		 */
		bctl->data.limit = limit_data;
		bctl->meta.limit = limit_meta;
		bctl->sys.limit = limit_sys;
	}
	key.objectid = BTRFS_FIRST_CHUNK_TREE_OBJECTID;
	key.offset = (u64)-1;
	key.type = BTRFS_CHUNK_ITEM_KEY;

	while (1) {
		if ((!counting && atomic_read(&fs_info->balance_pause_req)) ||
		    atomic_read(&fs_info->balance_cancel_req)) {
			ret = -ECANCELED;
			goto error;
		}

		mutex_lock(&fs_info->delete_unused_bgs_mutex);
		ret = btrfs_search_slot(NULL, chunk_root, &key, path, 0, 0);
		if (ret < 0) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			goto error;
		}

		/*
		 * this shouldn't happen, it means the last relocate
		 * failed
		 */
		if (ret == 0)
			BUG(); /* FIXME break ? */

		ret = btrfs_previous_item(chunk_root, path, 0,
					  BTRFS_CHUNK_ITEM_KEY);
		if (ret) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			ret = 0;
			break;
		}

		leaf = path->nodes[0];
		slot = path->slots[0];
		btrfs_item_key_to_cpu(leaf, &found_key, slot);

		if (found_key.objectid != key.objectid) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			break;
		}

		chunk = btrfs_item_ptr(leaf, slot, struct btrfs_chunk);
		chunk_type = btrfs_chunk_type(leaf, chunk);

		if (!counting) {
			spin_lock(&fs_info->balance_lock);
			bctl->stat.considered++;
			spin_unlock(&fs_info->balance_lock);
		}

		ret = should_balance_chunk(fs_info, leaf, chunk,
					   found_key.offset);

		btrfs_release_path(path);
		if (!ret) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			goto loop;
		}

		if (counting) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			spin_lock(&fs_info->balance_lock);
			bctl->stat.expected++;
			spin_unlock(&fs_info->balance_lock);

			if (chunk_type & BTRFS_BLOCK_GROUP_DATA)
				count_data++;
			else if (chunk_type & BTRFS_BLOCK_GROUP_SYSTEM)
				count_sys++;
			else if (chunk_type & BTRFS_BLOCK_GROUP_METADATA)
				count_meta++;

			goto loop;
		}

		/*
		 * Apply limit_min filter, no need to check if the LIMITS
		 * filter is used, limit_min is 0 by default
		 */
		if (((chunk_type & BTRFS_BLOCK_GROUP_DATA) &&
					count_data < bctl->data.limit_min)
				|| ((chunk_type & BTRFS_BLOCK_GROUP_METADATA) &&
					count_meta < bctl->meta.limit_min)
				|| ((chunk_type & BTRFS_BLOCK_GROUP_SYSTEM) &&
					count_sys < bctl->sys.limit_min)) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			goto loop;
		}

		if (!chunk_reserved) {
			/*
			 * We may be relocating the only data chunk we have,
			 * which could potentially end up with losing data's
			 * raid profile, so lets allocate an empty one in
			 * advance.
			 */
			ret = btrfs_may_alloc_data_chunk(fs_info,
							 found_key.offset);
			if (ret < 0) {
				mutex_unlock(&fs_info->delete_unused_bgs_mutex);
				goto error;
			} else if (ret == 1) {
				chunk_reserved = 1;
			}
		}

		ret = btrfs_relocate_chunk(fs_info, found_key.offset);
		mutex_unlock(&fs_info->delete_unused_bgs_mutex);
		if (ret == -ENOSPC) {
			enospc_errors++;
		} else if (ret == -ETXTBSY) {
			btrfs_info(fs_info,
	   "skipping relocation of block group %llu due to active swapfile",
				   found_key.offset);
			ret = 0;
		} else if (ret) {
			goto error;
		} else {
			spin_lock(&fs_info->balance_lock);
			bctl->stat.completed++;
			spin_unlock(&fs_info->balance_lock);
		}
loop:
		if (found_key.offset == 0)
			break;
		key.offset = found_key.offset - 1;
	}

	if (counting) {
		btrfs_release_path(path);
		counting = false;
		goto again;
	}
error:
	btrfs_free_path(path);
	if (enospc_errors) {
		btrfs_info(fs_info, "%d enospc errors during balance",
			   enospc_errors);
		if (!ret)
			ret = -ENOSPC;
	}

	return ret;
}