int btrfs_alloc_chunk(struct btrfs_trans_handle *trans, u64 type)
{
	u64 chunk_offset;

	lockdep_assert_held(&trans->fs_info->chunk_mutex);
	chunk_offset = find_next_chunk(trans->fs_info);
	return __btrfs_alloc_chunk(trans, chunk_offset, type);
}