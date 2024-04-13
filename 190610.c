}

static inline void f2fs_set_page_private(struct page *page,
						unsigned long data)
{
	if (PagePrivate(page))
		return;

	get_page(page);
	SetPagePrivate(page);