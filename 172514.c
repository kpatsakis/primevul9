static u64 get_full_stripe_logical(struct btrfs_block_group_cache *cache,
				   u64 bytenr)
{
	u64 ret;

	/*
	 * Due to chunk item size limit, full stripe length should not be
	 * larger than U32_MAX. Just a sanity check here.
	 */
	WARN_ON_ONCE(cache->full_stripe_len >= U32_MAX);

	/*
	 * round_down() can only handle power of 2, while RAID56 full
	 * stripe length can be 64KiB * n, so we need to manually round down.
	 */
	ret = div64_u64(bytenr - cache->key.objectid, cache->full_stripe_len) *
		cache->full_stripe_len + cache->key.objectid;
	return ret;
}