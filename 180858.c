static void pageset_set_high_and_batch(struct zone *zone,
				       struct per_cpu_pageset *pcp)
{
	if (percpu_pagelist_fraction)
		pageset_set_high(pcp,
			(zone->managed_pages /
				percpu_pagelist_fraction));
	else
		pageset_set_batch(pcp, zone_batchsize(zone));
}