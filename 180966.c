void __free_pages(struct page *page, unsigned int order)
{
	if (put_page_testzero(page)) {
		if (order == 0)
			free_unref_page(page);
		else
			__free_pages_ok(page, order);
	}
}