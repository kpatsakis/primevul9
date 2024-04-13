void si_meminfo_node(struct sysinfo *val, int nid)
{
	int zone_type;		/* needs to be signed */
	unsigned long managed_pages = 0;
	unsigned long managed_highpages = 0;
	unsigned long free_highpages = 0;
	pg_data_t *pgdat = NODE_DATA(nid);

	for (zone_type = 0; zone_type < MAX_NR_ZONES; zone_type++)
		managed_pages += pgdat->node_zones[zone_type].managed_pages;
	val->totalram = managed_pages;
	val->sharedram = node_page_state(pgdat, NR_SHMEM);
	val->freeram = sum_zone_node_page_state(nid, NR_FREE_PAGES);
#ifdef CONFIG_HIGHMEM
	for (zone_type = 0; zone_type < MAX_NR_ZONES; zone_type++) {
		struct zone *zone = &pgdat->node_zones[zone_type];

		if (is_highmem(zone)) {
			managed_highpages += zone->managed_pages;
			free_highpages += zone_page_state(zone, NR_FREE_PAGES);
		}
	}
	val->totalhigh = managed_highpages;
	val->freehigh = free_highpages;
#else
	val->totalhigh = managed_highpages;
	val->freehigh = free_highpages;
#endif
	val->mem_unit = PAGE_SIZE;
}