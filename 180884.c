void __meminit memmap_init_zone(unsigned long size, int nid, unsigned long zone,
		unsigned long start_pfn, enum memmap_context context)
{
	struct vmem_altmap *altmap = to_vmem_altmap(__pfn_to_phys(start_pfn));
	unsigned long end_pfn = start_pfn + size;
	pg_data_t *pgdat = NODE_DATA(nid);
	unsigned long pfn;
	unsigned long nr_initialised = 0;
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
	struct memblock_region *r = NULL, *tmp;
#endif

	if (highest_memmap_pfn < end_pfn - 1)
		highest_memmap_pfn = end_pfn - 1;

	/*
	 * Honor reservation requested by the driver for this ZONE_DEVICE
	 * memory
	 */
	if (altmap && start_pfn == altmap->base_pfn)
		start_pfn += altmap->reserve;

	for (pfn = start_pfn; pfn < end_pfn; pfn++) {
		/*
		 * There can be holes in boot-time mem_map[]s handed to this
		 * function.  They do not exist on hotplugged memory.
		 */
		if (context != MEMMAP_EARLY)
			goto not_early;

		if (!early_pfn_valid(pfn)) {
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
			/*
			 * Skip to the pfn preceding the next valid one (or
			 * end_pfn), such that we hit a valid pfn (or end_pfn)
			 * on our next iteration of the loop.
			 */
			pfn = memblock_next_valid_pfn(pfn, end_pfn) - 1;
#endif
			continue;
		}
		if (!early_pfn_in_nid(pfn, nid))
			continue;
		if (!update_defer_init(pgdat, pfn, end_pfn, &nr_initialised))
			break;

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
		/*
		 * Check given memblock attribute by firmware which can affect
		 * kernel memory layout.  If zone==ZONE_MOVABLE but memory is
		 * mirrored, it's an overlapped memmap init. skip it.
		 */
		if (mirrored_kernelcore && zone == ZONE_MOVABLE) {
			if (!r || pfn >= memblock_region_memory_end_pfn(r)) {
				for_each_memblock(memory, tmp)
					if (pfn < memblock_region_memory_end_pfn(tmp))
						break;
				r = tmp;
			}
			if (pfn >= memblock_region_memory_base_pfn(r) &&
			    memblock_is_mirror(r)) {
				/* already initialized as NORMAL */
				pfn = memblock_region_memory_end_pfn(r);
				continue;
			}
		}
#endif

not_early:
		/*
		 * Mark the block movable so that blocks are reserved for
		 * movable at startup. This will force kernel allocations
		 * to reserve their blocks rather than leaking throughout
		 * the address space during boot when many long-lived
		 * kernel allocations are made.
		 *
		 * bitmap is created for zone's valid pfn range. but memmap
		 * can be created for invalid pages (for alignment)
		 * check here not to call set_pageblock_migratetype() against
		 * pfn out of zone.
		 */
		if (!(pfn & (pageblock_nr_pages - 1))) {
			struct page *page = pfn_to_page(pfn);

			__init_single_page(page, pfn, zone, nid);
			set_pageblock_migratetype(page, MIGRATE_MOVABLE);
			cond_resched();
		} else {
			__init_single_pfn(pfn, zone, nid);
		}
	}
}