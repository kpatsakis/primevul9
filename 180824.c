void set_zone_contiguous(struct zone *zone)
{
	unsigned long block_start_pfn = zone->zone_start_pfn;
	unsigned long block_end_pfn;

	block_end_pfn = ALIGN(block_start_pfn + 1, pageblock_nr_pages);
	for (; block_start_pfn < zone_end_pfn(zone);
			block_start_pfn = block_end_pfn,
			 block_end_pfn += pageblock_nr_pages) {

		block_end_pfn = min(block_end_pfn, zone_end_pfn(zone));

		if (!__pageblock_pfn_to_page(block_start_pfn,
					     block_end_pfn, zone))
			return;
	}

	/* We confirm that there is no hole */
	zone->contiguous = true;
}