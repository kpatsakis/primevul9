static int btrfs_rm_dev_item(struct btrfs_fs_info *fs_info,
			     struct btrfs_device *device)
{
	struct btrfs_root *root = fs_info->chunk_root;
	int ret;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct btrfs_trans_handle *trans;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		btrfs_free_path(path);
		return PTR_ERR(trans);
	}
	key.objectid = BTRFS_DEV_ITEMS_OBJECTID;
	key.type = BTRFS_DEV_ITEM_KEY;
	key.offset = device->devid;

	ret = btrfs_search_slot(trans, root, &key, path, -1, 1);
	if (ret) {
		if (ret > 0)
			ret = -ENOENT;
		btrfs_abort_transaction(trans, ret);
		btrfs_end_transaction(trans);
		goto out;
	}

	ret = btrfs_del_item(trans, root, path);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		btrfs_end_transaction(trans);
	}

out:
	btrfs_free_path(path);
	if (!ret)
		ret = btrfs_commit_transaction(trans);
	return ret;
}