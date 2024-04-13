static int btrfs_check_uuid_tree_entry(struct btrfs_fs_info *fs_info,
				       u8 *uuid, u8 type, u64 subid)
{
	struct btrfs_key key;
	int ret = 0;
	struct btrfs_root *subvol_root;

	if (type != BTRFS_UUID_KEY_SUBVOL &&
	    type != BTRFS_UUID_KEY_RECEIVED_SUBVOL)
		goto out;

	key.objectid = subid;
	key.type = BTRFS_ROOT_ITEM_KEY;
	key.offset = (u64)-1;
	subvol_root = btrfs_read_fs_root_no_name(fs_info, &key);
	if (IS_ERR(subvol_root)) {
		ret = PTR_ERR(subvol_root);
		if (ret == -ENOENT)
			ret = 1;
		goto out;
	}

	switch (type) {
	case BTRFS_UUID_KEY_SUBVOL:
		if (memcmp(uuid, subvol_root->root_item.uuid, BTRFS_UUID_SIZE))
			ret = 1;
		break;
	case BTRFS_UUID_KEY_RECEIVED_SUBVOL:
		if (memcmp(uuid, subvol_root->root_item.received_uuid,
			   BTRFS_UUID_SIZE))
			ret = 1;
		break;
	}

out:
	return ret;
}