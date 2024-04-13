static noinline int init_first_rw_device(struct btrfs_trans_handle *trans,
					 struct btrfs_fs_info *fs_info)
{
	u64 chunk_offset;
	u64 sys_chunk_offset;
	u64 alloc_profile;
	int ret;

	chunk_offset = find_next_chunk(fs_info);
	alloc_profile = btrfs_metadata_alloc_profile(fs_info);
	ret = __btrfs_alloc_chunk(trans, chunk_offset, alloc_profile);
	if (ret)
		return ret;

	sys_chunk_offset = find_next_chunk(fs_info);
	alloc_profile = btrfs_system_alloc_profile(fs_info);
	ret = __btrfs_alloc_chunk(trans, sys_chunk_offset, alloc_profile);
	return ret;
}