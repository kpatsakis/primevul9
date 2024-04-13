void __page_frag_cache_drain(struct page *page, unsigned int count)
{
	VM_BUG_ON_PAGE(page_ref_count(page) == 0, page);

	if (page_ref_sub_and_test(page, count)) {
		unsigned int order = compound_order(page);

		if (order == 0)
			free_unref_page(page);
		else
			__free_pages_ok(page, order);
	}
}