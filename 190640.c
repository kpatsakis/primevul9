}

static inline void f2fs_clear_page_private(struct page *page)
{
	if (!PagePrivate(page))
		return;

	set_page_private(page, 0);
	ClearPagePrivate(page);