struct page *f2fs_get_lock_data_page(struct inode *inode, pgoff_t index,
							bool for_write)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;
repeat:
	page = f2fs_get_read_data_page(inode, index, 0, for_write);
	if (IS_ERR(page))
		return page;

	/* wait for read completion */
	lock_page(page);
	if (unlikely(page->mapping != mapping)) {
		f2fs_put_page(page, 1);
		goto repeat;
	}
	if (unlikely(!PageUptodate(page))) {
		f2fs_put_page(page, 1);
		return ERR_PTR(-EIO);
	}
	return page;
}