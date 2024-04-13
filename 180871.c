bool is_free_buddy_page(struct page *page)
{
	struct zone *zone = page_zone(page);
	unsigned long pfn = page_to_pfn(page);
	unsigned long flags;
	unsigned int order;

	spin_lock_irqsave(&zone->lock, flags);
	for (order = 0; order < MAX_ORDER; order++) {
		struct page *page_head = page - (pfn & ((1 << order) - 1));

		if (PageBuddy(page_head) && page_order(page_head) >= order)
			break;
	}
	spin_unlock_irqrestore(&zone->lock, flags);

	return order < MAX_ORDER;
}