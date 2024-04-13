static int btrfs_may_alloc_data_chunk(struct btrfs_fs_info *fs_info,
				      u64 chunk_offset)
{
	struct btrfs_block_group_cache *cache;
	u64 bytes_used;
	u64 chunk_type;

	cache = btrfs_lookup_block_group(fs_info, chunk_offset);
	ASSERT(cache);
	chunk_type = cache->flags;
	btrfs_put_block_group(cache);

	if (chunk_type & BTRFS_BLOCK_GROUP_DATA) {
		spin_lock(&fs_info->data_sinfo->lock);
		bytes_used = fs_info->data_sinfo->bytes_used;
		spin_unlock(&fs_info->data_sinfo->lock);

		if (!bytes_used) {
			struct btrfs_trans_handle *trans;
			int ret;

			trans =	btrfs_join_transaction(fs_info->tree_root);
			if (IS_ERR(trans))
				return PTR_ERR(trans);

			ret = btrfs_force_chunk_alloc(trans,
						      BTRFS_BLOCK_GROUP_DATA);
			btrfs_end_transaction(trans);
			if (ret < 0)
				return ret;

			btrfs_add_raid_kobjects(fs_info);

			return 1;
		}
	}
	return 0;
}