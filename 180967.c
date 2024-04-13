static void __meminit init_reserved_page(unsigned long pfn)
{
	pg_data_t *pgdat;
	int nid, zid;

	if (!early_page_uninitialised(pfn))
		return;

	nid = early_pfn_to_nid(pfn);
	pgdat = NODE_DATA(nid);

	for (zid = 0; zid < MAX_NR_ZONES; zid++) {
		struct zone *zone = &pgdat->node_zones[zid];

		if (pfn >= zone->zone_start_pfn && pfn < zone_end_pfn(zone))
			break;
	}
	__init_single_pfn(pfn, zid, nid);
}