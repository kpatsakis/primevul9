static void drain_pages_zone(unsigned int cpu, struct zone *zone)
{
	unsigned long flags;
	struct per_cpu_pageset *pset;
	struct per_cpu_pages *pcp;

	local_irq_save(flags);
	pset = per_cpu_ptr(zone->pageset, cpu);

	pcp = &pset->pcp;
	if (pcp->count) {
		free_pcppages_bulk(zone, pcp->count, pcp);
		pcp->count = 0;
	}
	local_irq_restore(flags);
}