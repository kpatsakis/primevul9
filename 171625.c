static void ttm_pages_put(struct page *pages[], unsigned npages,
		unsigned int order)
{
	unsigned int i, pages_nr = (1 << order);

	if (order == 0) {
		if (ttm_set_pages_array_wb(pages, npages))
			pr_err("Failed to set %d pages to wb!\n", npages);
	}

	for (i = 0; i < npages; ++i) {
		if (order > 0) {
			if (ttm_set_pages_wb(pages[i], pages_nr))
				pr_err("Failed to set %d pages to wb!\n", pages_nr);
		}
		__free_pages(pages[i], order);
	}
}