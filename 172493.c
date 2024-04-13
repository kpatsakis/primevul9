static int chunk_drange_filter(struct extent_buffer *leaf,
			       struct btrfs_chunk *chunk,
			       struct btrfs_balance_args *bargs)
{
	struct btrfs_stripe *stripe;
	int num_stripes = btrfs_chunk_num_stripes(leaf, chunk);
	u64 stripe_offset;
	u64 stripe_length;
	int factor;
	int i;

	if (!(bargs->flags & BTRFS_BALANCE_ARGS_DEVID))
		return 0;

	if (btrfs_chunk_type(leaf, chunk) & (BTRFS_BLOCK_GROUP_DUP |
	     BTRFS_BLOCK_GROUP_RAID1 | BTRFS_BLOCK_GROUP_RAID10)) {
		factor = num_stripes / 2;
	} else if (btrfs_chunk_type(leaf, chunk) & BTRFS_BLOCK_GROUP_RAID5) {
		factor = num_stripes - 1;
	} else if (btrfs_chunk_type(leaf, chunk) & BTRFS_BLOCK_GROUP_RAID6) {
		factor = num_stripes - 2;
	} else {
		factor = num_stripes;
	}

	for (i = 0; i < num_stripes; i++) {
		stripe = btrfs_stripe_nr(chunk, i);
		if (btrfs_stripe_devid(leaf, stripe) != bargs->devid)
			continue;

		stripe_offset = btrfs_stripe_offset(leaf, stripe);
		stripe_length = btrfs_chunk_length(leaf, chunk);
		stripe_length = div_u64(stripe_length, factor);

		if (stripe_offset < bargs->pend &&
		    stripe_offset + stripe_length > bargs->pstart)
			return 0;
	}

	return 1;
}