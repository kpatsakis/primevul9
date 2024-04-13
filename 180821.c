static struct page *rmqueue_pcplist(struct zone *preferred_zone,
			struct zone *zone, unsigned int order,
			gfp_t gfp_flags, int migratetype)
{
	struct per_cpu_pages *pcp;
	struct list_head *list;
	struct page *page;
	unsigned long flags;

	local_irq_save(flags);
	pcp = &this_cpu_ptr(zone->pageset)->pcp;
	list = &pcp->lists[migratetype];
	page = __rmqueue_pcplist(zone,  migratetype, pcp, list);
	if (page) {
		__count_zid_vm_events(PGALLOC, page_zonenum(page), 1 << order);
		zone_statistics(preferred_zone, zone);
	}
	local_irq_restore(flags);
	return page;
}