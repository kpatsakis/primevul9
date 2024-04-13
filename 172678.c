int btrfs_verify_dev_extents(struct btrfs_fs_info *fs_info)
{
	struct btrfs_path *path;
	struct btrfs_root *root = fs_info->dev_root;
	struct btrfs_key key;
	u64 prev_devid = 0;
	u64 prev_dev_ext_end = 0;
	int ret = 0;

	key.objectid = 1;
	key.type = BTRFS_DEV_EXTENT_KEY;
	key.offset = 0;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	path->reada = READA_FORWARD;
	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0)
		goto out;

	if (path->slots[0] >= btrfs_header_nritems(path->nodes[0])) {
		ret = btrfs_next_item(root, path);
		if (ret < 0)
			goto out;
		/* No dev extents at all? Not good */
		if (ret > 0) {
			ret = -EUCLEAN;
			goto out;
		}
	}
	while (1) {
		struct extent_buffer *leaf = path->nodes[0];
		struct btrfs_dev_extent *dext;
		int slot = path->slots[0];
		u64 chunk_offset;
		u64 physical_offset;
		u64 physical_len;
		u64 devid;

		btrfs_item_key_to_cpu(leaf, &key, slot);
		if (key.type != BTRFS_DEV_EXTENT_KEY)
			break;
		devid = key.objectid;
		physical_offset = key.offset;

		dext = btrfs_item_ptr(leaf, slot, struct btrfs_dev_extent);
		chunk_offset = btrfs_dev_extent_chunk_offset(leaf, dext);
		physical_len = btrfs_dev_extent_length(leaf, dext);

		/* Check if this dev extent overlaps with the previous one */
		if (devid == prev_devid && physical_offset < prev_dev_ext_end) {
			btrfs_err(fs_info,
"dev extent devid %llu physical offset %llu overlap with previous dev extent end %llu",
				  devid, physical_offset, prev_dev_ext_end);
			ret = -EUCLEAN;
			goto out;
		}

		ret = verify_one_dev_extent(fs_info, chunk_offset, devid,
					    physical_offset, physical_len);
		if (ret < 0)
			goto out;
		prev_devid = devid;
		prev_dev_ext_end = physical_offset + physical_len;

		ret = btrfs_next_item(root, path);
		if (ret < 0)
			goto out;
		if (ret > 0) {
			ret = 0;
			break;
		}
	}

	/* Ensure all chunks have corresponding dev extents */
	ret = verify_chunk_dev_extent_mapping(fs_info);
out:
	btrfs_free_path(path);
	return ret;
}