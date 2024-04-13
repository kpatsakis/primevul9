void prep_compound_page(struct page *page, unsigned int order)
{
	int i;
	int nr_pages = 1 << order;

	set_compound_page_dtor(page, COMPOUND_PAGE_DTOR);
	set_compound_order(page, order);
	__SetPageHead(page);
	for (i = 1; i < nr_pages; i++) {
		struct page *p = page + i;
		set_page_count(p, 0);
		p->mapping = TAIL_MAPPING;
		set_compound_head(p, page);
	}
	atomic_set(compound_mapcount_ptr(page), -1);
}