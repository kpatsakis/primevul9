noinline int btrfs_leaf_free_space(struct extent_buffer *leaf)
{
	struct btrfs_fs_info *fs_info = leaf->fs_info;
	int nritems = btrfs_header_nritems(leaf);
	int ret;

	ret = BTRFS_LEAF_DATA_SIZE(fs_info) - leaf_space_used(leaf, 0, nritems);
	if (ret < 0) {
		btrfs_crit(fs_info,
			   "leaf free space ret %d, leaf data size %lu, used %d nritems %d",
			   ret,
			   (unsigned long) BTRFS_LEAF_DATA_SIZE(fs_info),
			   leaf_space_used(leaf, 0, nritems), nritems);
	}
	return ret;
}