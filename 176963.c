int btrfs_split_item(struct btrfs_trans_handle *trans,
		     struct btrfs_root *root,
		     struct btrfs_path *path,
		     const struct btrfs_key *new_key,
		     unsigned long split_offset)
{
	int ret;
	ret = setup_leaf_for_split(trans, root, path,
				   sizeof(struct btrfs_item));
	if (ret)
		return ret;

	ret = split_item(path, new_key, split_offset);
	return ret;
}