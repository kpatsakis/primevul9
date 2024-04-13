static int btrfs_relocate_sys_chunks(struct btrfs_fs_info *fs_info)
{
	struct btrfs_root *chunk_root = fs_info->chunk_root;
	struct btrfs_path *path;
	struct extent_buffer *leaf;
	struct btrfs_chunk *chunk;
	struct btrfs_key key;
	struct btrfs_key found_key;
	u64 chunk_type;
	bool retried = false;
	int failed = 0;
	int ret;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

again:
	key.objectid = BTRFS_FIRST_CHUNK_TREE_OBJECTID;
	key.offset = (u64)-1;
	key.type = BTRFS_CHUNK_ITEM_KEY;

	while (1) {
		mutex_lock(&fs_info->delete_unused_bgs_mutex);
		ret = btrfs_search_slot(NULL, chunk_root, &key, path, 0, 0);
		if (ret < 0) {
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
			goto error;
		}
		BUG_ON(ret == 0); /* Corruption */

		ret = btrfs_previous_item(chunk_root, path, key.objectid,
					  key.type);
		if (ret)
			mutex_unlock(&fs_info->delete_unused_bgs_mutex);
		if (ret < 0)
			goto error;
		if (ret > 0)
			break;

		leaf = path->nodes[0];
		btrfs_item_key_to_cpu(leaf, &found_key, path->slots[0]);

		chunk = btrfs_item_ptr(leaf, path->slots[0],
				       struct btrfs_chunk);
		chunk_type = btrfs_chunk_type(leaf, chunk);
		btrfs_release_path(path);

		if (chunk_type & BTRFS_BLOCK_GROUP_SYSTEM) {
			ret = btrfs_relocate_chunk(fs_info, found_key.offset);
			if (ret == -ENOSPC)
				failed++;
			else
				BUG_ON(ret);
		}
		mutex_unlock(&fs_info->delete_unused_bgs_mutex);

		if (found_key.offset == 0)
			break;
		key.offset = found_key.offset - 1;
	}
	ret = 0;
	if (failed && !retried) {
		failed = 0;
		retried = true;
		goto again;
	} else if (WARN_ON(failed && retried)) {
		ret = -ENOSPC;
	}
error:
	btrfs_free_path(path);
	return ret;
}