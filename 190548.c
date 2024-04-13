static int f2fs_read_single_page(struct inode *inode, struct page *page,
					unsigned nr_pages,
					struct f2fs_map_blocks *map,
					struct bio **bio_ret,
					sector_t *last_block_in_bio,
					bool is_readahead)
{
	struct bio *bio = *bio_ret;
	const unsigned blkbits = inode->i_blkbits;
	const unsigned blocksize = 1 << blkbits;
	sector_t block_in_file;
	sector_t last_block;
	sector_t last_block_in_file;
	sector_t block_nr;
	int ret = 0;

	block_in_file = (sector_t)page_index(page);
	last_block = block_in_file + nr_pages;
	last_block_in_file = (i_size_read(inode) + blocksize - 1) >>
							blkbits;
	if (last_block > last_block_in_file)
		last_block = last_block_in_file;

	/* just zeroing out page which is beyond EOF */
	if (block_in_file >= last_block)
		goto zero_out;
	/*
	 * Map blocks using the previous result first.
	 */
	if ((map->m_flags & F2FS_MAP_MAPPED) &&
			block_in_file > map->m_lblk &&
			block_in_file < (map->m_lblk + map->m_len))
		goto got_it;

	/*
	 * Then do more f2fs_map_blocks() calls until we are
	 * done with this page.
	 */
	map->m_lblk = block_in_file;
	map->m_len = last_block - block_in_file;

	ret = f2fs_map_blocks(inode, map, 0, F2FS_GET_BLOCK_DEFAULT);
	if (ret)
		goto out;
got_it:
	if ((map->m_flags & F2FS_MAP_MAPPED)) {
		block_nr = map->m_pblk + block_in_file - map->m_lblk;
		SetPageMappedToDisk(page);

		if (!PageUptodate(page) && (!PageSwapCache(page) &&
					!cleancache_get_page(page))) {
			SetPageUptodate(page);
			goto confused;
		}

		if (!f2fs_is_valid_blkaddr(F2FS_I_SB(inode), block_nr,
						DATA_GENERIC_ENHANCE_READ)) {
			ret = -EFSCORRUPTED;
			goto out;
		}
	} else {
zero_out:
		zero_user_segment(page, 0, PAGE_SIZE);
		if (!PageUptodate(page))
			SetPageUptodate(page);
		unlock_page(page);
		goto out;
	}

	/*
	 * This page will go to BIO.  Do we need to send this
	 * BIO off first?
	 */
	if (bio && (*last_block_in_bio != block_nr - 1 ||
		!__same_bdev(F2FS_I_SB(inode), block_nr, bio))) {
submit_and_realloc:
		__submit_bio(F2FS_I_SB(inode), bio, DATA);
		bio = NULL;
	}
	if (bio == NULL) {
		bio = f2fs_grab_read_bio(inode, block_nr, nr_pages,
				is_readahead ? REQ_RAHEAD : 0);
		if (IS_ERR(bio)) {
			ret = PTR_ERR(bio);
			bio = NULL;
			goto out;
		}
	}

	/*
	 * If the page is under writeback, we need to wait for
	 * its completion to see the correct decrypted data.
	 */
	f2fs_wait_on_block_writeback(inode, block_nr);

	if (bio_add_page(bio, page, blocksize, 0) < blocksize)
		goto submit_and_realloc;

	inc_page_count(F2FS_I_SB(inode), F2FS_RD_DATA);
	ClearPageError(page);
	*last_block_in_bio = block_nr;
	goto out;
confused:
	if (bio) {
		__submit_bio(F2FS_I_SB(inode), bio, DATA);
		bio = NULL;
	}
	unlock_page(page);
out:
	*bio_ret = bio;
	return ret;
}