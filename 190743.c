void f2fs_clear_page_cache_dirty_tag(struct page *page)
{
	struct address_space *mapping = page_mapping(page);
	unsigned long flags;

	xa_lock_irqsave(&mapping->i_pages, flags);
	__xa_clear_mark(&mapping->i_pages, page_index(page),
						PAGECACHE_TAG_DIRTY);
	xa_unlock_irqrestore(&mapping->i_pages, flags);
}