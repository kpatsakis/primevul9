static struct extent_buffer *alloc_tree_block_no_bg_flush(
					  struct btrfs_trans_handle *trans,
					  struct btrfs_root *root,
					  u64 parent_start,
					  const struct btrfs_disk_key *disk_key,
					  int level,
					  u64 hint,
					  u64 empty_size,
					  enum btrfs_lock_nesting nest)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct extent_buffer *ret;

	/*
	 * If we are COWing a node/leaf from the extent, chunk, device or free
	 * space trees, make sure that we do not finish block group creation of
	 * pending block groups. We do this to avoid a deadlock.
	 * COWing can result in allocation of a new chunk, and flushing pending
	 * block groups (btrfs_create_pending_block_groups()) can be triggered
	 * when finishing allocation of a new chunk. Creation of a pending block
	 * group modifies the extent, chunk, device and free space trees,
	 * therefore we could deadlock with ourselves since we are holding a
	 * lock on an extent buffer that btrfs_create_pending_block_groups() may
	 * try to COW later.
	 * For similar reasons, we also need to delay flushing pending block
	 * groups when splitting a leaf or node, from one of those trees, since
	 * we are holding a write lock on it and its parent or when inserting a
	 * new root node for one of those trees.
	 */
	if (root == fs_info->extent_root ||
	    root == fs_info->chunk_root ||
	    root == fs_info->dev_root ||
	    root == fs_info->free_space_root)
		trans->can_flush_pending_bgs = false;

	ret = btrfs_alloc_tree_block(trans, root, parent_start,
				     root->root_key.objectid, disk_key, level,
				     hint, empty_size, nest);
	trans->can_flush_pending_bgs = true;

	return ret;
}