static int f2fs_write_cache_pages(struct address_space *mapping,
					struct writeback_control *wbc,
					enum iostat_type io_type)
{
	int ret = 0;
	int done = 0;
	struct pagevec pvec;
	struct f2fs_sb_info *sbi = F2FS_M_SB(mapping);
	struct bio *bio = NULL;
	sector_t last_block;
	int nr_pages;
	pgoff_t uninitialized_var(writeback_index);
	pgoff_t index;
	pgoff_t end;		/* Inclusive */
	pgoff_t done_index;
	int cycled;
	int range_whole = 0;
	xa_mark_t tag;
	int nwritten = 0;

	pagevec_init(&pvec);

	if (get_dirty_pages(mapping->host) <=
				SM_I(F2FS_M_SB(mapping))->min_hot_blocks)
		set_inode_flag(mapping->host, FI_HOT_DATA);
	else
		clear_inode_flag(mapping->host, FI_HOT_DATA);

	if (wbc->range_cyclic) {
		writeback_index = mapping->writeback_index; /* prev offset */
		index = writeback_index;
		if (index == 0)
			cycled = 1;
		else
			cycled = 0;
		end = -1;
	} else {
		index = wbc->range_start >> PAGE_SHIFT;
		end = wbc->range_end >> PAGE_SHIFT;
		if (wbc->range_start == 0 && wbc->range_end == LLONG_MAX)
			range_whole = 1;
		cycled = 1; /* ignore range_cyclic tests */
	}
	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag = PAGECACHE_TAG_TOWRITE;
	else
		tag = PAGECACHE_TAG_DIRTY;
retry:
	if (wbc->sync_mode == WB_SYNC_ALL || wbc->tagged_writepages)
		tag_pages_for_writeback(mapping, index, end);
	done_index = index;
	while (!done && (index <= end)) {
		int i;

		nr_pages = pagevec_lookup_range_tag(&pvec, mapping, &index, end,
				tag);
		if (nr_pages == 0)
			break;

		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
			bool submitted = false;

			/* give a priority to WB_SYNC threads */
			if (atomic_read(&sbi->wb_sync_req[DATA]) &&
					wbc->sync_mode == WB_SYNC_NONE) {
				done = 1;
				break;
			}

			done_index = page->index;
retry_write:
			lock_page(page);

			if (unlikely(page->mapping != mapping)) {
continue_unlock:
				unlock_page(page);
				continue;
			}

			if (!PageDirty(page)) {
				/* someone wrote it for us */
				goto continue_unlock;
			}

			if (PageWriteback(page)) {
				if (wbc->sync_mode != WB_SYNC_NONE) {
					f2fs_wait_on_page_writeback(page,
							DATA, true, true);
					f2fs_submit_ipu_bio(sbi, &bio, page);
				} else {
					goto continue_unlock;
				}
			}

			if (!clear_page_dirty_for_io(page))
				goto continue_unlock;

			ret = __write_data_page(page, &submitted, &bio,
					&last_block, wbc, io_type);
			if (unlikely(ret)) {
				/*
				 * keep nr_to_write, since vfs uses this to
				 * get # of written pages.
				 */
				if (ret == AOP_WRITEPAGE_ACTIVATE) {
					unlock_page(page);
					ret = 0;
					continue;
				} else if (ret == -EAGAIN) {
					ret = 0;
					if (wbc->sync_mode == WB_SYNC_ALL) {
						cond_resched();
						congestion_wait(BLK_RW_ASYNC,
									HZ/50);
						goto retry_write;
					}
					continue;
				}
				done_index = page->index + 1;
				done = 1;
				break;
			} else if (submitted) {
				nwritten++;
			}

			if (--wbc->nr_to_write <= 0 &&
					wbc->sync_mode == WB_SYNC_NONE) {
				done = 1;
				break;
			}
		}
		pagevec_release(&pvec);
		cond_resched();
	}

	if (!cycled && !done) {
		cycled = 1;
		index = 0;
		end = writeback_index - 1;
		goto retry;
	}
	if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
		mapping->writeback_index = done_index;

	if (nwritten)
		f2fs_submit_merged_write_cond(F2FS_M_SB(mapping), mapping->host,
								NULL, 0, DATA);
	/* submit cached bio of IPU write */
	if (bio)
		__submit_bio(sbi, bio, DATA);

	return ret;
}