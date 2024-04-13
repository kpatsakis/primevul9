static void __ref alloc_node_mem_map(struct pglist_data *pgdat)
{
	unsigned long __maybe_unused start = 0;
	unsigned long __maybe_unused offset = 0;

	/* Skip empty nodes */
	if (!pgdat->node_spanned_pages)
		return;

#ifdef CONFIG_FLAT_NODE_MEM_MAP
	start = pgdat->node_start_pfn & ~(MAX_ORDER_NR_PAGES - 1);
	offset = pgdat->node_start_pfn - start;
	/* ia64 gets its own node_mem_map, before this, without bootmem */
	if (!pgdat->node_mem_map) {
		unsigned long size, end;
		struct page *map;

		/*
		 * The zone's endpoints aren't required to be MAX_ORDER
		 * aligned but the node_mem_map endpoints must be in order
		 * for the buddy allocator to function correctly.
		 */
		end = pgdat_end_pfn(pgdat);
		end = ALIGN(end, MAX_ORDER_NR_PAGES);
		size =  (end - start) * sizeof(struct page);
		map = alloc_remap(pgdat->node_id, size);
		if (!map)
			map = memblock_virt_alloc_node_nopanic(size,
							       pgdat->node_id);
		pgdat->node_mem_map = map + offset;
	}
#ifndef CONFIG_NEED_MULTIPLE_NODES
	/*
	 * With no DISCONTIG, the global mem_map is just set as node 0's
	 */
	if (pgdat == NODE_DATA(0)) {
		mem_map = NODE_DATA(0)->node_mem_map;
#if defined(CONFIG_HAVE_MEMBLOCK_NODE_MAP) || defined(CONFIG_FLATMEM)
		if (page_to_pfn(mem_map) != pgdat->node_start_pfn)
			mem_map -= offset;
#endif /* CONFIG_HAVE_MEMBLOCK_NODE_MAP */
	}
#endif
#endif /* CONFIG_FLAT_NODE_MEM_MAP */
}