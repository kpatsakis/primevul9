static int f2fs_write_end(struct file *file,
			struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = page->mapping->host;

	trace_f2fs_write_end(inode, pos, len, copied);

	/*
	 * This should be come from len == PAGE_SIZE, and we expect copied
	 * should be PAGE_SIZE. Otherwise, we treat it with zero copied and
	 * let generic_perform_write() try to copy data again through copied=0.
	 */
	if (!PageUptodate(page)) {
		if (unlikely(copied != len))
			copied = 0;
		else
			SetPageUptodate(page);
	}
	if (!copied)
		goto unlock_out;

	set_page_dirty(page);

	if (pos + copied > i_size_read(inode))
		f2fs_i_size_write(inode, pos + copied);
unlock_out:
	f2fs_put_page(page, 1);
	f2fs_update_time(F2FS_I_SB(inode), REQ_TIME);
	return copied;
}