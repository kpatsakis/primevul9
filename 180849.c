static void __init deferred_free_range(unsigned long pfn,
				       unsigned long nr_pages)
{
	struct page *page;
	unsigned long i;

	if (!nr_pages)
		return;

	page = pfn_to_page(pfn);

	/* Free a large naturally-aligned chunk if possible */
	if (nr_pages == pageblock_nr_pages &&
	    (pfn & (pageblock_nr_pages - 1)) == 0) {
		set_pageblock_migratetype(page, MIGRATE_MOVABLE);
		__free_pages_boot_core(page, pageblock_order);
		return;
	}

	for (i = 0; i < nr_pages; i++, page++, pfn++) {
		if ((pfn & (pageblock_nr_pages - 1)) == 0)
			set_pageblock_migratetype(page, MIGRATE_MOVABLE);
		__free_pages_boot_core(page, 0);
	}
}