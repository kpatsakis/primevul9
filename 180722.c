static void __paginginit free_area_init_core(struct pglist_data *pgdat)
{
	enum zone_type j;
	int nid = pgdat->node_id;

	pgdat_resize_init(pgdat);
#ifdef CONFIG_NUMA_BALANCING
	spin_lock_init(&pgdat->numabalancing_migrate_lock);
	pgdat->numabalancing_migrate_nr_pages = 0;
	pgdat->numabalancing_migrate_next_window = jiffies;
#endif
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	spin_lock_init(&pgdat->split_queue_lock);
	INIT_LIST_HEAD(&pgdat->split_queue);
	pgdat->split_queue_len = 0;
#endif
	init_waitqueue_head(&pgdat->kswapd_wait);
	init_waitqueue_head(&pgdat->pfmemalloc_wait);
#ifdef CONFIG_COMPACTION
	init_waitqueue_head(&pgdat->kcompactd_wait);
#endif
	pgdat_page_ext_init(pgdat);
	spin_lock_init(&pgdat->lru_lock);
	lruvec_init(node_lruvec(pgdat));

	pgdat->per_cpu_nodestats = &boot_nodestats;

	for (j = 0; j < MAX_NR_ZONES; j++) {
		struct zone *zone = pgdat->node_zones + j;
		unsigned long size, realsize, freesize, memmap_pages;
		unsigned long zone_start_pfn = zone->zone_start_pfn;

		size = zone->spanned_pages;
		realsize = freesize = zone->present_pages;

		/*
		 * Adjust freesize so that it accounts for how much memory
		 * is used by this zone for memmap. This affects the watermark
		 * and per-cpu initialisations
		 */
		memmap_pages = calc_memmap_size(size, realsize);
		if (!is_highmem_idx(j)) {
			if (freesize >= memmap_pages) {
				freesize -= memmap_pages;
				if (memmap_pages)
					printk(KERN_DEBUG
					       "  %s zone: %lu pages used for memmap\n",
					       zone_names[j], memmap_pages);
			} else
				pr_warn("  %s zone: %lu pages exceeds freesize %lu\n",
					zone_names[j], memmap_pages, freesize);
		}

		/* Account for reserved pages */
		if (j == 0 && freesize > dma_reserve) {
			freesize -= dma_reserve;
			printk(KERN_DEBUG "  %s zone: %lu pages reserved\n",
					zone_names[0], dma_reserve);
		}

		if (!is_highmem_idx(j))
			nr_kernel_pages += freesize;
		/* Charge for highmem memmap if there are enough kernel pages */
		else if (nr_kernel_pages > memmap_pages * 2)
			nr_kernel_pages -= memmap_pages;
		nr_all_pages += freesize;

		/*
		 * Set an approximate value for lowmem here, it will be adjusted
		 * when the bootmem allocator frees pages into the buddy system.
		 * And all highmem pages will be managed by the buddy system.
		 */
		zone->managed_pages = is_highmem_idx(j) ? realsize : freesize;
#ifdef CONFIG_NUMA
		zone->node = nid;
#endif
		zone->name = zone_names[j];
		zone->zone_pgdat = pgdat;
		spin_lock_init(&zone->lock);
		zone_seqlock_init(zone);
		zone_pcp_init(zone);

		if (!size)
			continue;

		set_pageblock_order();
		setup_usemap(pgdat, zone, zone_start_pfn, size);
		init_currently_empty_zone(zone, zone_start_pfn, size);
		memmap_init(size, nid, j, zone_start_pfn);
	}
}