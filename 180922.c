static int page_is_consistent(struct zone *zone, struct page *page)
{
	if (!pfn_valid_within(page_to_pfn(page)))
		return 0;
	if (zone != page_zone(page))
		return 0;

	return 1;
}