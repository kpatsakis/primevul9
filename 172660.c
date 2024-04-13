static int should_balance_chunk(struct btrfs_fs_info *fs_info,
				struct extent_buffer *leaf,
				struct btrfs_chunk *chunk, u64 chunk_offset)
{
	struct btrfs_balance_control *bctl = fs_info->balance_ctl;
	struct btrfs_balance_args *bargs = NULL;
	u64 chunk_type = btrfs_chunk_type(leaf, chunk);

	/* type filter */
	if (!((chunk_type & BTRFS_BLOCK_GROUP_TYPE_MASK) &
	      (bctl->flags & BTRFS_BALANCE_TYPE_MASK))) {
		return 0;
	}

	if (chunk_type & BTRFS_BLOCK_GROUP_DATA)
		bargs = &bctl->data;
	else if (chunk_type & BTRFS_BLOCK_GROUP_SYSTEM)
		bargs = &bctl->sys;
	else if (chunk_type & BTRFS_BLOCK_GROUP_METADATA)
		bargs = &bctl->meta;

	/* profiles filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_PROFILES) &&
	    chunk_profiles_filter(chunk_type, bargs)) {
		return 0;
	}

	/* usage filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_USAGE) &&
	    chunk_usage_filter(fs_info, chunk_offset, bargs)) {
		return 0;
	} else if ((bargs->flags & BTRFS_BALANCE_ARGS_USAGE_RANGE) &&
	    chunk_usage_range_filter(fs_info, chunk_offset, bargs)) {
		return 0;
	}

	/* devid filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_DEVID) &&
	    chunk_devid_filter(leaf, chunk, bargs)) {
		return 0;
	}

	/* drange filter, makes sense only with devid filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_DRANGE) &&
	    chunk_drange_filter(leaf, chunk, bargs)) {
		return 0;
	}

	/* vrange filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_VRANGE) &&
	    chunk_vrange_filter(leaf, chunk, chunk_offset, bargs)) {
		return 0;
	}

	/* stripes filter */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_STRIPES_RANGE) &&
	    chunk_stripes_range_filter(leaf, chunk, bargs)) {
		return 0;
	}

	/* soft profile changing mode */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_SOFT) &&
	    chunk_soft_convert_filter(chunk_type, bargs)) {
		return 0;
	}

	/*
	 * limited by count, must be the last filter
	 */
	if ((bargs->flags & BTRFS_BALANCE_ARGS_LIMIT)) {
		if (bargs->limit == 0)
			return 0;
		else
			bargs->limit--;
	} else if ((bargs->flags & BTRFS_BALANCE_ARGS_LIMIT_RANGE)) {
		/*
		 * Same logic as the 'limit' filter; the minimum cannot be
		 * determined here because we do not have the global information
		 * about the count of all chunks that satisfy the filters.
		 */
		if (bargs->limit_max == 0)
			return 0;
		else
			bargs->limit_max--;
	}

	return 1;
}