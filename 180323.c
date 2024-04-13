static bool is_dump_unreclaim_slabs(void)
{
	unsigned long nr_lru;

	nr_lru = global_node_page_state(NR_ACTIVE_ANON) +
		 global_node_page_state(NR_INACTIVE_ANON) +
		 global_node_page_state(NR_ACTIVE_FILE) +
		 global_node_page_state(NR_INACTIVE_FILE) +
		 global_node_page_state(NR_ISOLATED_ANON) +
		 global_node_page_state(NR_ISOLATED_FILE) +
		 global_node_page_state(NR_UNEVICTABLE);

	return (global_node_page_state(NR_SLAB_UNRECLAIMABLE) > nr_lru);
}