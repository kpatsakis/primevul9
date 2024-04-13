void free_compound_page(struct page *page)
{
	__free_pages_ok(page, compound_order(page));
}