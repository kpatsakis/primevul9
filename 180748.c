static bool check_new_pages(struct page *page, unsigned int order)
{
	int i;
	for (i = 0; i < (1 << order); i++) {
		struct page *p = page + i;

		if (unlikely(check_new_page(p)))
			return true;
	}

	return false;
}