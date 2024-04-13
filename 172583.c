static int btrfs_free_dev_extent(struct btrfs_trans_handle *trans,
			  struct btrfs_device *device,
			  u64 start, u64 *dev_extent_len)
{
	struct btrfs_fs_info *fs_info = device->fs_info;
	struct btrfs_root *root = fs_info->dev_root;
	int ret;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct btrfs_key found_key;
	struct extent_buffer *leaf = NULL;
	struct btrfs_dev_extent *extent = NULL;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	key.objectid = device->devid;
	key.offset = start;
	key.type = BTRFS_DEV_EXTENT_KEY;
again:
	ret = btrfs_search_slot(trans, root, &key, path, -1, 1);
	if (ret > 0) {
		ret = btrfs_previous_item(root, path, key.objectid,
					  BTRFS_DEV_EXTENT_KEY);
		if (ret)
			goto out;
		leaf = path->nodes[0];
		btrfs_item_key_to_cpu(leaf, &found_key, path->slots[0]);
		extent = btrfs_item_ptr(leaf, path->slots[0],
					struct btrfs_dev_extent);
		BUG_ON(found_key.offset > start || found_key.offset +
		       btrfs_dev_extent_length(leaf, extent) < start);
		key = found_key;
		btrfs_release_path(path);
		goto again;
	} else if (ret == 0) {
		leaf = path->nodes[0];
		extent = btrfs_item_ptr(leaf, path->slots[0],
					struct btrfs_dev_extent);
	} else {
		btrfs_handle_fs_error(fs_info, ret, "Slot search failed");
		goto out;
	}

	*dev_extent_len = btrfs_dev_extent_length(leaf, extent);

	ret = btrfs_del_item(trans, root, path);
	if (ret) {
		btrfs_handle_fs_error(fs_info, ret,
				      "Failed to remove dev extent item");
	} else {
		set_bit(BTRFS_TRANS_HAVE_FREE_BGS, &trans->transaction->flags);
	}
out:
	btrfs_free_path(path);
	return ret;
}