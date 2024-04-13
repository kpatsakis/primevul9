int btrfs_defrag_file(struct inode *inode, struct file *file,
		      struct btrfs_ioctl_defrag_range_args *range,
		      u64 newer_than, unsigned long max_to_defrag)
{
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct file_ra_state *ra = NULL;
	unsigned long last_index;
	u64 isize = i_size_read(inode);
	u64 last_len = 0;
	u64 skip = 0;
	u64 defrag_end = 0;
	u64 newer_off = range->start;
	unsigned long i;
	unsigned long ra_index = 0;
	int ret;
	int defrag_count = 0;
	int compress_type = BTRFS_COMPRESS_ZLIB;
	u32 extent_thresh = range->extent_thresh;
	unsigned long max_cluster = SZ_256K >> PAGE_SHIFT;
	unsigned long cluster = max_cluster;
	u64 new_align = ~((u64)SZ_128K - 1);
	struct page **pages = NULL;
	bool do_compress = range->flags & BTRFS_DEFRAG_RANGE_COMPRESS;

	if (isize == 0)
		return 0;

	if (range->start >= isize)
		return -EINVAL;

	if (do_compress) {
		if (range->compress_type > BTRFS_COMPRESS_TYPES)
			return -EINVAL;
		if (range->compress_type)
			compress_type = range->compress_type;
	}

	if (extent_thresh == 0)
		extent_thresh = SZ_256K;

	/*
	 * If we were not given a file, allocate a readahead context. As
	 * readahead is just an optimization, defrag will work without it so
	 * we don't error out.
	 */
	if (!file) {
		ra = kzalloc(sizeof(*ra), GFP_KERNEL);
		if (ra)
			file_ra_state_init(ra, inode->i_mapping);
	} else {
		ra = &file->f_ra;
	}

	pages = kmalloc_array(max_cluster, sizeof(struct page *), GFP_KERNEL);
	if (!pages) {
		ret = -ENOMEM;
		goto out_ra;
	}

	/* find the last page to defrag */
	if (range->start + range->len > range->start) {
		last_index = min_t(u64, isize - 1,
			 range->start + range->len - 1) >> PAGE_SHIFT;
	} else {
		last_index = (isize - 1) >> PAGE_SHIFT;
	}

	if (newer_than) {
		ret = find_new_extents(root, inode, newer_than,
				       &newer_off, SZ_64K);
		if (!ret) {
			range->start = newer_off;
			/*
			 * we always align our defrag to help keep
			 * the extents in the file evenly spaced
			 */
			i = (newer_off & new_align) >> PAGE_SHIFT;
		} else
			goto out_ra;
	} else {
		i = range->start >> PAGE_SHIFT;
	}
	if (!max_to_defrag)
		max_to_defrag = last_index - i + 1;

	/*
	 * make writeback starts from i, so the defrag range can be
	 * written sequentially.
	 */
	if (i < inode->i_mapping->writeback_index)
		inode->i_mapping->writeback_index = i;

	while (i <= last_index && defrag_count < max_to_defrag &&
	       (i < DIV_ROUND_UP(i_size_read(inode), PAGE_SIZE))) {
		/*
		 * make sure we stop running if someone unmounts
		 * the FS
		 */
		if (!(inode->i_sb->s_flags & SB_ACTIVE))
			break;

		if (btrfs_defrag_cancelled(fs_info)) {
			btrfs_debug(fs_info, "defrag_file cancelled");
			ret = -EAGAIN;
			break;
		}

		if (!should_defrag_range(inode, (u64)i << PAGE_SHIFT,
					 extent_thresh, &last_len, &skip,
					 &defrag_end, do_compress)){
			unsigned long next;
			/*
			 * the should_defrag function tells us how much to skip
			 * bump our counter by the suggested amount
			 */
			next = DIV_ROUND_UP(skip, PAGE_SIZE);
			i = max(i + 1, next);
			continue;
		}

		if (!newer_than) {
			cluster = (PAGE_ALIGN(defrag_end) >>
				   PAGE_SHIFT) - i;
			cluster = min(cluster, max_cluster);
		} else {
			cluster = max_cluster;
		}

		if (i + cluster > ra_index) {
			ra_index = max(i, ra_index);
			if (ra)
				page_cache_sync_readahead(inode->i_mapping, ra,
						file, ra_index, cluster);
			ra_index += cluster;
		}

		inode_lock(inode);
		if (IS_SWAPFILE(inode)) {
			ret = -ETXTBSY;
		} else {
			if (do_compress)
				BTRFS_I(inode)->defrag_compress = compress_type;
			ret = cluster_pages_for_defrag(inode, pages, i, cluster);
		}
		if (ret < 0) {
			inode_unlock(inode);
			goto out_ra;
		}

		defrag_count += ret;
		balance_dirty_pages_ratelimited(inode->i_mapping);
		inode_unlock(inode);

		if (newer_than) {
			if (newer_off == (u64)-1)
				break;

			if (ret > 0)
				i += ret;

			newer_off = max(newer_off + 1,
					(u64)i << PAGE_SHIFT);

			ret = find_new_extents(root, inode, newer_than,
					       &newer_off, SZ_64K);
			if (!ret) {
				range->start = newer_off;
				i = (newer_off & new_align) >> PAGE_SHIFT;
			} else {
				break;
			}
		} else {
			if (ret > 0) {
				i += ret;
				last_len += ret << PAGE_SHIFT;
			} else {
				i++;
				last_len = 0;
			}
		}
	}

	if ((range->flags & BTRFS_DEFRAG_RANGE_START_IO)) {
		filemap_flush(inode->i_mapping);
		if (test_bit(BTRFS_INODE_HAS_ASYNC_EXTENT,
			     &BTRFS_I(inode)->runtime_flags))
			filemap_flush(inode->i_mapping);
	}

	if (range->compress_type == BTRFS_COMPRESS_LZO) {
		btrfs_set_fs_incompat(fs_info, COMPRESS_LZO);
	} else if (range->compress_type == BTRFS_COMPRESS_ZSTD) {
		btrfs_set_fs_incompat(fs_info, COMPRESS_ZSTD);
	}

	ret = defrag_count;

out_ra:
	if (do_compress) {
		inode_lock(inode);
		BTRFS_I(inode)->defrag_compress = BTRFS_COMPRESS_NONE;
		inode_unlock(inode);
	}
	if (!file)
		kfree(ra);
	kfree(pages);
	return ret;
}