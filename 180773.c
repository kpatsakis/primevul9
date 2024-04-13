static void __meminit __init_single_page(struct page *page, unsigned long pfn,
				unsigned long zone, int nid)
{
	mm_zero_struct_page(page);
	set_page_links(page, zone, nid, pfn);
	init_page_count(page);
	page_mapcount_reset(page);
	page_cpupid_reset_last(page);

	INIT_LIST_HEAD(&page->lru);
#ifdef WANT_PAGE_VIRTUAL
	/* The shift won't overflow because ZONE_NORMAL is below 4G. */
	if (!is_highmem_idx(zone))
		set_page_address(page, __va(pfn << PAGE_SHIFT));
#endif
}