static int page_outside_zone_boundaries(struct zone *zone, struct page *page)
{
	int ret = 0;
	unsigned seq;
	unsigned long pfn = page_to_pfn(page);
	unsigned long sp, start_pfn;

	do {
		seq = zone_span_seqbegin(zone);
		start_pfn = zone->zone_start_pfn;
		sp = zone->spanned_pages;
		if (!zone_spans_pfn(zone, pfn))
			ret = 1;
	} while (zone_span_seqretry(zone, seq));

	if (ret)
		pr_err("page 0x%lx outside node %d zone %s [ 0x%lx - 0x%lx ]\n",
			pfn, zone_to_nid(zone), zone->name,
			start_pfn, start_pfn + sp);

	return ret;
}