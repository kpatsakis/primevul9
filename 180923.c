static unsigned long __paginginit calc_memmap_size(unsigned long spanned_pages,
						   unsigned long present_pages)
{
	unsigned long pages = spanned_pages;

	/*
	 * Provide a more accurate estimation if there are holes within
	 * the zone and SPARSEMEM is in use. If there are holes within the
	 * zone, each populated memory region may cost us one or two extra
	 * memmap pages due to alignment because memmap pages for each
	 * populated regions may not be naturally aligned on page boundary.
	 * So the (present_pages >> 4) heuristic is a tradeoff for that.
	 */
	if (spanned_pages > present_pages + (present_pages >> 4) &&
	    IS_ENABLED(CONFIG_SPARSEMEM))
		pages = present_pages;

	return PAGE_ALIGN(pages * sizeof(struct page)) >> PAGE_SHIFT;
}