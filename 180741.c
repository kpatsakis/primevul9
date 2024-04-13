void show_free_areas(unsigned int filter, nodemask_t *nodemask)
{
	unsigned long free_pcp = 0;
	int cpu;
	struct zone *zone;
	pg_data_t *pgdat;

	for_each_populated_zone(zone) {
		if (show_mem_node_skip(filter, zone_to_nid(zone), nodemask))
			continue;

		for_each_online_cpu(cpu)
			free_pcp += per_cpu_ptr(zone->pageset, cpu)->pcp.count;
	}

	printk("active_anon:%lu inactive_anon:%lu isolated_anon:%lu\n"
		" active_file:%lu inactive_file:%lu isolated_file:%lu\n"
		" unevictable:%lu dirty:%lu writeback:%lu unstable:%lu\n"
		" slab_reclaimable:%lu slab_unreclaimable:%lu\n"
		" mapped:%lu shmem:%lu pagetables:%lu bounce:%lu\n"
		" free:%lu free_pcp:%lu free_cma:%lu\n",
		global_node_page_state(NR_ACTIVE_ANON),
		global_node_page_state(NR_INACTIVE_ANON),
		global_node_page_state(NR_ISOLATED_ANON),
		global_node_page_state(NR_ACTIVE_FILE),
		global_node_page_state(NR_INACTIVE_FILE),
		global_node_page_state(NR_ISOLATED_FILE),
		global_node_page_state(NR_UNEVICTABLE),
		global_node_page_state(NR_FILE_DIRTY),
		global_node_page_state(NR_WRITEBACK),
		global_node_page_state(NR_UNSTABLE_NFS),
		global_node_page_state(NR_SLAB_RECLAIMABLE),
		global_node_page_state(NR_SLAB_UNRECLAIMABLE),
		global_node_page_state(NR_FILE_MAPPED),
		global_node_page_state(NR_SHMEM),
		global_zone_page_state(NR_PAGETABLE),
		global_zone_page_state(NR_BOUNCE),
		global_zone_page_state(NR_FREE_PAGES),
		free_pcp,
		global_zone_page_state(NR_FREE_CMA_PAGES));

	for_each_online_pgdat(pgdat) {
		if (show_mem_node_skip(filter, pgdat->node_id, nodemask))
			continue;

		printk("Node %d"
			" active_anon:%lukB"
			" inactive_anon:%lukB"
			" active_file:%lukB"
			" inactive_file:%lukB"
			" unevictable:%lukB"
			" isolated(anon):%lukB"
			" isolated(file):%lukB"
			" mapped:%lukB"
			" dirty:%lukB"
			" writeback:%lukB"
			" shmem:%lukB"
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
			" shmem_thp: %lukB"
			" shmem_pmdmapped: %lukB"
			" anon_thp: %lukB"
#endif
			" writeback_tmp:%lukB"
			" unstable:%lukB"
			" all_unreclaimable? %s"
			"\n",
			pgdat->node_id,
			K(node_page_state(pgdat, NR_ACTIVE_ANON)),
			K(node_page_state(pgdat, NR_INACTIVE_ANON)),
			K(node_page_state(pgdat, NR_ACTIVE_FILE)),
			K(node_page_state(pgdat, NR_INACTIVE_FILE)),
			K(node_page_state(pgdat, NR_UNEVICTABLE)),
			K(node_page_state(pgdat, NR_ISOLATED_ANON)),
			K(node_page_state(pgdat, NR_ISOLATED_FILE)),
			K(node_page_state(pgdat, NR_FILE_MAPPED)),
			K(node_page_state(pgdat, NR_FILE_DIRTY)),
			K(node_page_state(pgdat, NR_WRITEBACK)),
			K(node_page_state(pgdat, NR_SHMEM)),
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
			K(node_page_state(pgdat, NR_SHMEM_THPS) * HPAGE_PMD_NR),
			K(node_page_state(pgdat, NR_SHMEM_PMDMAPPED)
					* HPAGE_PMD_NR),
			K(node_page_state(pgdat, NR_ANON_THPS) * HPAGE_PMD_NR),
#endif
			K(node_page_state(pgdat, NR_WRITEBACK_TEMP)),
			K(node_page_state(pgdat, NR_UNSTABLE_NFS)),
			pgdat->kswapd_failures >= MAX_RECLAIM_RETRIES ?
				"yes" : "no");
	}

	for_each_populated_zone(zone) {
		int i;

		if (show_mem_node_skip(filter, zone_to_nid(zone), nodemask))
			continue;

		free_pcp = 0;
		for_each_online_cpu(cpu)
			free_pcp += per_cpu_ptr(zone->pageset, cpu)->pcp.count;

		show_node(zone);
		printk(KERN_CONT
			"%s"
			" free:%lukB"
			" min:%lukB"
			" low:%lukB"
			" high:%lukB"
			" active_anon:%lukB"
			" inactive_anon:%lukB"
			" active_file:%lukB"
			" inactive_file:%lukB"
			" unevictable:%lukB"
			" writepending:%lukB"
			" present:%lukB"
			" managed:%lukB"
			" mlocked:%lukB"
			" kernel_stack:%lukB"
			" pagetables:%lukB"
			" bounce:%lukB"
			" free_pcp:%lukB"
			" local_pcp:%ukB"
			" free_cma:%lukB"
			"\n",
			zone->name,
			K(zone_page_state(zone, NR_FREE_PAGES)),
			K(min_wmark_pages(zone)),
			K(low_wmark_pages(zone)),
			K(high_wmark_pages(zone)),
			K(zone_page_state(zone, NR_ZONE_ACTIVE_ANON)),
			K(zone_page_state(zone, NR_ZONE_INACTIVE_ANON)),
			K(zone_page_state(zone, NR_ZONE_ACTIVE_FILE)),
			K(zone_page_state(zone, NR_ZONE_INACTIVE_FILE)),
			K(zone_page_state(zone, NR_ZONE_UNEVICTABLE)),
			K(zone_page_state(zone, NR_ZONE_WRITE_PENDING)),
			K(zone->present_pages),
			K(zone->managed_pages),
			K(zone_page_state(zone, NR_MLOCK)),
			zone_page_state(zone, NR_KERNEL_STACK_KB),
			K(zone_page_state(zone, NR_PAGETABLE)),
			K(zone_page_state(zone, NR_BOUNCE)),
			K(free_pcp),
			K(this_cpu_read(zone->pageset->pcp.count)),
			K(zone_page_state(zone, NR_FREE_CMA_PAGES)));
		printk("lowmem_reserve[]:");
		for (i = 0; i < MAX_NR_ZONES; i++)
			printk(KERN_CONT " %ld", zone->lowmem_reserve[i]);
		printk(KERN_CONT "\n");
	}

	for_each_populated_zone(zone) {
		unsigned int order;
		unsigned long nr[MAX_ORDER], flags, total = 0;
		unsigned char types[MAX_ORDER];

		if (show_mem_node_skip(filter, zone_to_nid(zone), nodemask))
			continue;
		show_node(zone);
		printk(KERN_CONT "%s: ", zone->name);

		spin_lock_irqsave(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++) {
			struct free_area *area = &zone->free_area[order];
			int type;

			nr[order] = area->nr_free;
			total += nr[order] << order;

			types[order] = 0;
			for (type = 0; type < MIGRATE_TYPES; type++) {
				if (!list_empty(&area->free_list[type]))
					types[order] |= 1 << type;
			}
		}
		spin_unlock_irqrestore(&zone->lock, flags);
		for (order = 0; order < MAX_ORDER; order++) {
			printk(KERN_CONT "%lu*%lukB ",
			       nr[order], K(1UL) << order);
			if (nr[order])
				show_migration_types(types[order]);
		}
		printk(KERN_CONT "= %lukB\n", K(total));
	}

	hugetlb_show_meminfo();

	printk("%ld total pagecache pages\n", global_node_page_state(NR_FILE_PAGES));

	show_swap_cache_info();
}