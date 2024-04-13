__offline_isolated_pages(unsigned long start_pfn, unsigned long end_pfn)
{
	struct page *page;
	struct zone *zone;
	unsigned int order, i;
	unsigned long pfn;
	unsigned long flags;
	/* find the first valid pfn */
	for (pfn = start_pfn; pfn < end_pfn; pfn++)
		if (pfn_valid(pfn))
			break;
	if (pfn == end_pfn)
		return;
	offline_mem_sections(pfn, end_pfn);
	zone = page_zone(pfn_to_page(pfn));
	spin_lock_irqsave(&zone->lock, flags);
	pfn = start_pfn;
	while (pfn < end_pfn) {
		if (!pfn_valid(pfn)) {
			pfn++;
			continue;
		}
		page = pfn_to_page(pfn);
		/*
		 * The HWPoisoned page may be not in buddy system, and
		 * page_count() is not 0.
		 */
		if (unlikely(!PageBuddy(page) && PageHWPoison(page))) {
			pfn++;
			SetPageReserved(page);
			continue;
		}

		BUG_ON(page_count(page));
		BUG_ON(!PageBuddy(page));
		order = page_order(page);
#ifdef CONFIG_DEBUG_VM
		pr_info("remove from free list %lx %d %lx\n",
			pfn, 1 << order, end_pfn);
#endif
		list_del(&page->lru);
		rmv_page_order(page);
		zone->free_area[order].nr_free--;
		for (i = 0; i < (1 << order); i++)
			SetPageReserved((page+i));
		pfn += (1 << order);
	}
	spin_unlock_irqrestore(&zone->lock, flags);
}