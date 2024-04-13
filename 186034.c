int __generic_block_fiemap(struct inode *inode,
			   struct fiemap_extent_info *fieinfo, loff_t start,
			   loff_t len, get_block_t *get_block)
{
	struct buffer_head map_bh;
	sector_t start_blk, last_blk;
	loff_t isize = i_size_read(inode);
	u64 logical = 0, phys = 0, size = 0;
	u32 flags = FIEMAP_EXTENT_MERGED;
	bool past_eof = false, whole_file = false;
	int ret = 0;

	ret = fiemap_check_flags(fieinfo, FIEMAP_FLAG_SYNC);
	if (ret)
		return ret;

	/*
	 * Either the i_mutex or other appropriate locking needs to be held
	 * since we expect isize to not change at all through the duration of
	 * this call.
	 */
	if (len >= isize) {
		whole_file = true;
		len = isize;
	}

	/*
	 * Some filesystems can't deal with being asked to map less than
	 * blocksize, so make sure our len is at least block length.
	 */
	if (logical_to_blk(inode, len) == 0)
		len = blk_to_logical(inode, 1);

	start_blk = logical_to_blk(inode, start);
	last_blk = logical_to_blk(inode, start + len - 1);

	do {
		/*
		 * we set b_size to the total size we want so it will map as
		 * many contiguous blocks as possible at once
		 */
		memset(&map_bh, 0, sizeof(struct buffer_head));
		map_bh.b_size = len;

		ret = get_block(inode, start_blk, &map_bh, 0);
		if (ret)
			break;

		/* HOLE */
		if (!buffer_mapped(&map_bh)) {
			start_blk++;

			/*
			 * We want to handle the case where there is an
			 * allocated block at the front of the file, and then
			 * nothing but holes up to the end of the file properly,
			 * to make sure that extent at the front gets properly
			 * marked with FIEMAP_EXTENT_LAST
			 */
			if (!past_eof &&
			    blk_to_logical(inode, start_blk) >= isize)
				past_eof = 1;

			/*
			 * First hole after going past the EOF, this is our
			 * last extent
			 */
			if (past_eof && size) {
				flags = FIEMAP_EXTENT_MERGED|FIEMAP_EXTENT_LAST;
				ret = fiemap_fill_next_extent(fieinfo, logical,
							      phys, size,
							      flags);
			} else if (size) {
				ret = fiemap_fill_next_extent(fieinfo, logical,
							      phys, size, flags);
				size = 0;
			}

			/* if we have holes up to/past EOF then we're done */
			if (start_blk > last_blk || past_eof || ret)
				break;
		} else {
			/*
			 * We have gone over the length of what we wanted to
			 * map, and it wasn't the entire file, so add the extent
			 * we got last time and exit.
			 *
			 * This is for the case where say we want to map all the
			 * way up to the second to the last block in a file, but
			 * the last block is a hole, making the second to last
			 * block FIEMAP_EXTENT_LAST.  In this case we want to
			 * see if there is a hole after the second to last block
			 * so we can mark it properly.  If we found data after
			 * we exceeded the length we were requesting, then we
			 * are good to go, just add the extent to the fieinfo
			 * and break
			 */
			if (start_blk > last_blk && !whole_file) {
				ret = fiemap_fill_next_extent(fieinfo, logical,
							      phys, size,
							      flags);
				break;
			}

			/*
			 * if size != 0 then we know we already have an extent
			 * to add, so add it.
			 */
			if (size) {
				ret = fiemap_fill_next_extent(fieinfo, logical,
							      phys, size,
							      flags);
				if (ret)
					break;
			}

			logical = blk_to_logical(inode, start_blk);
			phys = blk_to_logical(inode, map_bh.b_blocknr);
			size = map_bh.b_size;
			flags = FIEMAP_EXTENT_MERGED;

			start_blk += logical_to_blk(inode, size);

			/*
			 * If we are past the EOF, then we need to make sure as
			 * soon as we find a hole that the last extent we found
			 * is marked with FIEMAP_EXTENT_LAST
			 */
			if (!past_eof && logical + size >= isize)
				past_eof = true;
		}
		cond_resched();
		if (fatal_signal_pending(current)) {
			ret = -EINTR;
			break;
		}

	} while (1);

	/* If ret is 1 then we just hit the end of the extent array */
	if (ret == 1)
		ret = 0;

	return ret;
}