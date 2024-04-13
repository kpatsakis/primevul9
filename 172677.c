static int chunk_usage_range_filter(struct btrfs_fs_info *fs_info, u64 chunk_offset,
			      struct btrfs_balance_args *bargs)
{
	struct btrfs_block_group_cache *cache;
	u64 chunk_used;
	u64 user_thresh_min;
	u64 user_thresh_max;
	int ret = 1;

	cache = btrfs_lookup_block_group(fs_info, chunk_offset);
	chunk_used = btrfs_block_group_used(&cache->item);

	if (bargs->usage_min == 0)
		user_thresh_min = 0;
	else
		user_thresh_min = div_factor_fine(cache->key.offset,
					bargs->usage_min);

	if (bargs->usage_max == 0)
		user_thresh_max = 1;
	else if (bargs->usage_max > 100)
		user_thresh_max = cache->key.offset;
	else
		user_thresh_max = div_factor_fine(cache->key.offset,
					bargs->usage_max);

	if (user_thresh_min <= chunk_used && chunk_used < user_thresh_max)
		ret = 0;

	btrfs_put_block_group(cache);
	return ret;
}