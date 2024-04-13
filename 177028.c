int btrfs_previous_extent_item(struct btrfs_root *root,
			struct btrfs_path *path, u64 min_objectid)
{
	struct btrfs_key found_key;
	struct extent_buffer *leaf;
	u32 nritems;
	int ret;

	while (1) {
		if (path->slots[0] == 0) {
			ret = btrfs_prev_leaf(root, path);
			if (ret != 0)
				return ret;
		} else {
			path->slots[0]--;
		}
		leaf = path->nodes[0];
		nritems = btrfs_header_nritems(leaf);
		if (nritems == 0)
			return 1;
		if (path->slots[0] == nritems)
			path->slots[0]--;

		btrfs_item_key_to_cpu(leaf, &found_key, path->slots[0]);
		if (found_key.objectid < min_objectid)
			break;
		if (found_key.type == BTRFS_EXTENT_ITEM_KEY ||
		    found_key.type == BTRFS_METADATA_ITEM_KEY)
			return 0;
		if (found_key.objectid == min_objectid &&
		    found_key.type < BTRFS_EXTENT_ITEM_KEY)
			break;
	}
	return 1;
}