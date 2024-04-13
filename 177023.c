int btrfs_duplicate_item(struct btrfs_trans_handle *trans,
			 struct btrfs_root *root,
			 struct btrfs_path *path,
			 const struct btrfs_key *new_key)
{
	struct extent_buffer *leaf;
	int ret;
	u32 item_size;

	leaf = path->nodes[0];
	item_size = btrfs_item_size_nr(leaf, path->slots[0]);
	ret = setup_leaf_for_split(trans, root, path,
				   item_size + sizeof(struct btrfs_item));
	if (ret)
		return ret;

	path->slots[0]++;
	setup_items_for_insert(root, path, new_key, &item_size, 1);
	leaf = path->nodes[0];
	memcpy_extent_buffer(leaf,
			     btrfs_item_ptr_offset(leaf, path->slots[0]),
			     btrfs_item_ptr_offset(leaf, path->slots[0] - 1),
			     item_size);
	return 0;
}