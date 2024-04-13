int btrfs_read_chunk_tree(struct btrfs_fs_info *fs_info)
{
	struct btrfs_root *root = fs_info->chunk_root;
	struct btrfs_path *path;
	struct extent_buffer *leaf;
	struct btrfs_key key;
	struct btrfs_key found_key;
	int ret;
	int slot;
	u64 total_dev = 0;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	/*
	 * uuid_mutex is needed only if we are mounting a sprout FS
	 * otherwise we don't need it.
	 */
	mutex_lock(&uuid_mutex);
	mutex_lock(&fs_info->chunk_mutex);

	/*
	 * Read all device items, and then all the chunk items. All
	 * device items are found before any chunk item (their object id
	 * is smaller than the lowest possible object id for a chunk
	 * item - BTRFS_FIRST_CHUNK_TREE_OBJECTID).
	 */
	key.objectid = BTRFS_DEV_ITEMS_OBJECTID;
	key.offset = 0;
	key.type = 0;
	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0)
		goto error;
	while (1) {
		leaf = path->nodes[0];
		slot = path->slots[0];
		if (slot >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(root, path);
			if (ret == 0)
				continue;
			if (ret < 0)
				goto error;
			break;
		}
		btrfs_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.type == BTRFS_DEV_ITEM_KEY) {
			struct btrfs_dev_item *dev_item;
			dev_item = btrfs_item_ptr(leaf, slot,
						  struct btrfs_dev_item);
			ret = read_one_dev(fs_info, leaf, dev_item);
			if (ret)
				goto error;
			total_dev++;
		} else if (found_key.type == BTRFS_CHUNK_ITEM_KEY) {
			struct btrfs_chunk *chunk;
			chunk = btrfs_item_ptr(leaf, slot, struct btrfs_chunk);
			ret = read_one_chunk(fs_info, &found_key, leaf, chunk);
			if (ret)
				goto error;
		}
		path->slots[0]++;
	}

	/*
	 * After loading chunk tree, we've got all device information,
	 * do another round of validation checks.
	 */
	if (total_dev != fs_info->fs_devices->total_devices) {
		btrfs_err(fs_info,
	   "super_num_devices %llu mismatch with num_devices %llu found here",
			  btrfs_super_num_devices(fs_info->super_copy),
			  total_dev);
		ret = -EINVAL;
		goto error;
	}
	if (btrfs_super_total_bytes(fs_info->super_copy) <
	    fs_info->fs_devices->total_rw_bytes) {
		btrfs_err(fs_info,
	"super_total_bytes %llu mismatch with fs_devices total_rw_bytes %llu",
			  btrfs_super_total_bytes(fs_info->super_copy),
			  fs_info->fs_devices->total_rw_bytes);
		ret = -EINVAL;
		goto error;
	}
	ret = 0;
error:
	mutex_unlock(&fs_info->chunk_mutex);
	mutex_unlock(&uuid_mutex);

	btrfs_free_path(path);
	return ret;
}