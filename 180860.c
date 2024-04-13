static int __init deferred_init_memmap(void *data)
{
	pg_data_t *pgdat = data;
	int nid = pgdat->node_id;
	unsigned long start = jiffies;
	unsigned long nr_pages = 0;
	unsigned long spfn, epfn;
	phys_addr_t spa, epa;
	int zid;
	struct zone *zone;
	unsigned long first_init_pfn = pgdat->first_deferred_pfn;
	const struct cpumask *cpumask = cpumask_of_node(pgdat->node_id);
	u64 i;

	if (first_init_pfn == ULONG_MAX) {
		pgdat_init_report_one_done();
		return 0;
	}

	/* Bind memory initialisation thread to a local node if possible */
	if (!cpumask_empty(cpumask))
		set_cpus_allowed_ptr(current, cpumask);

	/* Sanity check boundaries */
	BUG_ON(pgdat->first_deferred_pfn < pgdat->node_start_pfn);
	BUG_ON(pgdat->first_deferred_pfn > pgdat_end_pfn(pgdat));
	pgdat->first_deferred_pfn = ULONG_MAX;

	/* Only the highest zone is deferred so find it */
	for (zid = 0; zid < MAX_NR_ZONES; zid++) {
		zone = pgdat->node_zones + zid;
		if (first_init_pfn < zone_end_pfn(zone))
			break;
	}
	first_init_pfn = max(zone->zone_start_pfn, first_init_pfn);

	for_each_free_mem_range(i, nid, MEMBLOCK_NONE, &spa, &epa, NULL) {
		spfn = max_t(unsigned long, first_init_pfn, PFN_UP(spa));
		epfn = min_t(unsigned long, zone_end_pfn(zone), PFN_DOWN(epa));
		nr_pages += deferred_init_range(nid, zid, spfn, epfn);
	}

	/* Sanity check that the next zone really is unpopulated */
	WARN_ON(++zid < MAX_NR_ZONES && populated_zone(++zone));

	pr_info("node %d initialised, %lu pages in %ums\n", nid, nr_pages,
					jiffies_to_msecs(jiffies - start));

	pgdat_init_report_one_done();
	return 0;
}