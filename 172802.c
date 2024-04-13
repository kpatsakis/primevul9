static int ext4_writepage(struct page *page,
			  struct writeback_control *wbc)
{
	int ret = 0;
	loff_t size;
	unsigned int len;
	struct buffer_head *page_bufs = NULL;
	struct inode *inode = page->mapping->host;
	struct ext4_io_submit io_submit;
	bool keep_towrite = false;

	if (unlikely(ext4_forced_shutdown(EXT4_SB(inode->i_sb)))) {
		ext4_invalidatepage(page, 0, PAGE_SIZE);
		unlock_page(page);
		return -EIO;
	}

	trace_ext4_writepage(page);
	size = i_size_read(inode);
	if (page->index == size >> PAGE_SHIFT &&
	    !ext4_verity_in_progress(inode))
		len = size & ~PAGE_MASK;
	else
		len = PAGE_SIZE;

	page_bufs = page_buffers(page);
	/*
	 * We cannot do block allocation or other extent handling in this
	 * function. If there are buffers needing that, we have to redirty
	 * the page. But we may reach here when we do a journal commit via
	 * journal_submit_inode_data_buffers() and in that case we must write
	 * allocated buffers to achieve data=ordered mode guarantees.
	 *
	 * Also, if there is only one buffer per page (the fs block
	 * size == the page size), if one buffer needs block
	 * allocation or needs to modify the extent tree to clear the
	 * unwritten flag, we know that the page can't be written at
	 * all, so we might as well refuse the write immediately.
	 * Unfortunately if the block size != page size, we can't as
	 * easily detect this case using ext4_walk_page_buffers(), but
	 * for the extremely common case, this is an optimization that
	 * skips a useless round trip through ext4_bio_write_page().
	 */
	if (ext4_walk_page_buffers(NULL, page_bufs, 0, len, NULL,
				   ext4_bh_delay_or_unwritten)) {
		redirty_page_for_writepage(wbc, page);
		if ((current->flags & PF_MEMALLOC) ||
		    (inode->i_sb->s_blocksize == PAGE_SIZE)) {
			/*
			 * For memory cleaning there's no point in writing only
			 * some buffers. So just bail out. Warn if we came here
			 * from direct reclaim.
			 */
			WARN_ON_ONCE((current->flags & (PF_MEMALLOC|PF_KSWAPD))
							== PF_MEMALLOC);
			unlock_page(page);
			return 0;
		}
		keep_towrite = true;
	}

	if (PageChecked(page) && ext4_should_journal_data(inode))
		/*
		 * It's mmapped pagecache.  Add buffers and journal it.  There
		 * doesn't seem much point in redirtying the page here.
		 */
		return __ext4_journalled_writepage(page, len);

	ext4_io_submit_init(&io_submit, wbc);
	io_submit.io_end = ext4_init_io_end(inode, GFP_NOFS);
	if (!io_submit.io_end) {
		redirty_page_for_writepage(wbc, page);
		unlock_page(page);
		return -ENOMEM;
	}
	ret = ext4_bio_write_page(&io_submit, page, len, wbc, keep_towrite);
	ext4_io_submit(&io_submit);
	/* Drop io_end reference we got from init */
	ext4_put_io_end_defer(io_submit.io_end);
	return ret;
}