static void reserve_highatomic_pageblock(struct page *page, struct zone *zone,
				unsigned int alloc_order)
{
	int mt;
	unsigned long max_managed, flags;

	/*
	 * Limit the number reserved to 1 pageblock or roughly 1% of a zone.
	 * Check is race-prone but harmless.
	 */
	max_managed = (zone->managed_pages / 100) + pageblock_nr_pages;
	if (zone->nr_reserved_highatomic >= max_managed)
		return;

	spin_lock_irqsave(&zone->lock, flags);

	/* Recheck the nr_reserved_highatomic limit under the lock */
	if (zone->nr_reserved_highatomic >= max_managed)
		goto out_unlock;

	/* Yoink! */
	mt = get_pageblock_migratetype(page);
	if (!is_migrate_highatomic(mt) && !is_migrate_isolate(mt)
	    && !is_migrate_cma(mt)) {
		zone->nr_reserved_highatomic += pageblock_nr_pages;
		set_pageblock_migratetype(page, MIGRATE_HIGHATOMIC);
		move_freepages_block(zone, page, MIGRATE_HIGHATOMIC, NULL);
	}

out_unlock:
	spin_unlock_irqrestore(&zone->lock, flags);
}