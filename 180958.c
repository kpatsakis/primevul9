bool is_pageblock_removable_nolock(struct page *page)
{
	struct zone *zone;
	unsigned long pfn;

	/*
	 * We have to be careful here because we are iterating over memory
	 * sections which are not zone aware so we might end up outside of
	 * the zone but still within the section.
	 * We have to take care about the node as well. If the node is offline
	 * its NODE_DATA will be NULL - see page_zone.
	 */
	if (!node_online(page_to_nid(page)))
		return false;

	zone = page_zone(page);
	pfn = page_to_pfn(page);
	if (!zone_spans_pfn(zone, pfn))
		return false;

	return !has_unmovable_pages(zone, page, 0, MIGRATE_MOVABLE, true);
}