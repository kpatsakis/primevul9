static int btrfs_relocate_chunk(struct btrfs_fs_info *fs_info, u64 chunk_offset)
{
	struct btrfs_root *root = fs_info->chunk_root;
	struct btrfs_trans_handle *trans;
	int ret;

	/*
	 * Prevent races with automatic removal of unused block groups.
	 * After we relocate and before we remove the chunk with offset
	 * chunk_offset, automatic removal of the block group can kick in,
	 * resulting in a failure when calling btrfs_remove_chunk() below.
	 *
	 * Make sure to acquire this mutex before doing a tree search (dev
	 * or chunk trees) to find chunks. Otherwise the cleaner kthread might
	 * call btrfs_remove_chunk() (through btrfs_delete_unused_bgs()) after
	 * we release the path used to search the chunk/dev tree and before
	 * the current task acquires this mutex and calls us.
	 */
	lockdep_assert_held(&fs_info->delete_unused_bgs_mutex);

	ret = btrfs_can_relocate(fs_info, chunk_offset);
	if (ret)
		return -ENOSPC;

	/* step one, relocate all the extents inside this chunk */
	btrfs_scrub_pause(fs_info);
	ret = btrfs_relocate_block_group(fs_info, chunk_offset);
	btrfs_scrub_continue(fs_info);
	if (ret)
		return ret;

	/*
	 * We add the kobjects here (and after forcing data chunk creation)
	 * since relocation is the only place we'll create chunks of a new
	 * type at runtime.  The only place where we'll remove the last
	 * chunk of a type is the call immediately below this one.  Even
	 * so, we're protected against races with the cleaner thread since
	 * we're covered by the delete_unused_bgs_mutex.
	 */
	btrfs_add_raid_kobjects(fs_info);

	trans = btrfs_start_trans_remove_block_group(root->fs_info,
						     chunk_offset);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		btrfs_handle_fs_error(root->fs_info, ret, NULL);
		return ret;
	}

	/*
	 * step two, delete the device extents and the
	 * chunk tree entries
	 */
	ret = btrfs_remove_chunk(trans, chunk_offset);
	btrfs_end_transaction(trans);
	return ret;
}