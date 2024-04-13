static int scrub_extent(struct scrub_ctx *sctx, struct map_lookup *map,
			u64 logical, u64 len,
			u64 physical, struct btrfs_device *dev, u64 flags,
			u64 gen, int mirror_num, u64 physical_for_dev_replace)
{
	int ret;
	u8 csum[BTRFS_CSUM_SIZE];
	u32 blocksize;

	if (flags & BTRFS_EXTENT_FLAG_DATA) {
		if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK)
			blocksize = map->stripe_len;
		else
			blocksize = sctx->fs_info->sectorsize;
		spin_lock(&sctx->stat_lock);
		sctx->stat.data_extents_scrubbed++;
		sctx->stat.data_bytes_scrubbed += len;
		spin_unlock(&sctx->stat_lock);
	} else if (flags & BTRFS_EXTENT_FLAG_TREE_BLOCK) {
		if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK)
			blocksize = map->stripe_len;
		else
			blocksize = sctx->fs_info->nodesize;
		spin_lock(&sctx->stat_lock);
		sctx->stat.tree_extents_scrubbed++;
		sctx->stat.tree_bytes_scrubbed += len;
		spin_unlock(&sctx->stat_lock);
	} else {
		blocksize = sctx->fs_info->sectorsize;
		WARN_ON(1);
	}

	while (len) {
		u64 l = min_t(u64, len, blocksize);
		int have_csum = 0;

		if (flags & BTRFS_EXTENT_FLAG_DATA) {
			/* push csums to sbio */
			have_csum = scrub_find_csum(sctx, logical, csum);
			if (have_csum == 0)
				++sctx->stat.no_csum;
		}
		ret = scrub_pages(sctx, logical, l, physical, dev, flags, gen,
				  mirror_num, have_csum ? csum : NULL, 0,
				  physical_for_dev_replace);
		if (ret)
			return ret;
		len -= l;
		logical += l;
		physical += l;
		physical_for_dev_replace += l;
	}
	return 0;
}