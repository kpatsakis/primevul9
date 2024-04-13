static int scrub_extent_for_parity(struct scrub_parity *sparity,
				   u64 logical, u64 len,
				   u64 physical, struct btrfs_device *dev,
				   u64 flags, u64 gen, int mirror_num)
{
	struct scrub_ctx *sctx = sparity->sctx;
	int ret;
	u8 csum[BTRFS_CSUM_SIZE];
	u32 blocksize;

	if (test_bit(BTRFS_DEV_STATE_MISSING, &dev->dev_state)) {
		scrub_parity_mark_sectors_error(sparity, logical, len);
		return 0;
	}

	if (flags & BTRFS_EXTENT_FLAG_DATA) {
		blocksize = sparity->stripe_len;
	} else if (flags & BTRFS_EXTENT_FLAG_TREE_BLOCK) {
		blocksize = sparity->stripe_len;
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
				goto skip;
		}
		ret = scrub_pages_for_parity(sparity, logical, l, physical, dev,
					     flags, gen, mirror_num,
					     have_csum ? csum : NULL);
		if (ret)
			return ret;
skip:
		len -= l;
		logical += l;
		physical += l;
	}
	return 0;
}