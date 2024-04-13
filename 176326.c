static int __gup_device_huge(unsigned long pfn, unsigned long addr,
			     unsigned long end, unsigned int flags,
			     struct page **pages, int *nr)
{
	int nr_start = *nr;
	struct dev_pagemap *pgmap = NULL;

	do {
		struct page *page = pfn_to_page(pfn);

		pgmap = get_dev_pagemap(pfn, pgmap);
		if (unlikely(!pgmap)) {
			undo_dev_pagemap(nr, nr_start, flags, pages);
			return 0;
		}
		SetPageReferenced(page);
		pages[*nr] = page;
		if (unlikely(!try_grab_page(page, flags))) {
			undo_dev_pagemap(nr, nr_start, flags, pages);
			return 0;
		}
		(*nr)++;
		pfn++;
	} while (addr += PAGE_SIZE, addr != end);

	if (pgmap)
		put_dev_pagemap(pgmap);
	return 1;
}