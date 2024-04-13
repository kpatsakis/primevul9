static inline void reset_deferred_meminit(pg_data_t *pgdat)
{
	unsigned long max_initialise;
	unsigned long reserved_lowmem;

	/*
	 * Initialise at least 2G of a node but also take into account that
	 * two large system hashes that can take up 1GB for 0.25TB/node.
	 */
	max_initialise = max(2UL << (30 - PAGE_SHIFT),
		(pgdat->node_spanned_pages >> 8));

	/*
	 * Compensate the all the memblock reservations (e.g. crash kernel)
	 * from the initial estimation to make sure we will initialize enough
	 * memory to boot.
	 */
	reserved_lowmem = memblock_reserved_memory_within(pgdat->node_start_pfn,
			pgdat->node_start_pfn + max_initialise);
	max_initialise += reserved_lowmem;

	pgdat->static_init_size = min(max_initialise, pgdat->node_spanned_pages);
	pgdat->first_deferred_pfn = ULONG_MAX;
}