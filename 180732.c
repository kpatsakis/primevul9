void __init setup_per_cpu_pageset(void)
{
	struct pglist_data *pgdat;
	struct zone *zone;

	for_each_populated_zone(zone)
		setup_zone_pageset(zone);

	for_each_online_pgdat(pgdat)
		pgdat->per_cpu_nodestats =
			alloc_percpu(struct per_cpu_nodestat);
}