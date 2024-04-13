void __init page_alloc_init_late(void)
{
	struct zone *zone;

#ifdef CONFIG_DEFERRED_STRUCT_PAGE_INIT
	int nid;

	/* There will be num_node_state(N_MEMORY) threads */
	atomic_set(&pgdat_init_n_undone, num_node_state(N_MEMORY));
	for_each_node_state(nid, N_MEMORY) {
		kthread_run(deferred_init_memmap, NODE_DATA(nid), "pgdatinit%d", nid);
	}

	/* Block until all are initialised */
	wait_for_completion(&pgdat_init_all_done_comp);

	/* Reinit limits that are based on free pages after the kernel is up */
	files_maxfiles_init();
#endif
#ifdef CONFIG_ARCH_DISCARD_MEMBLOCK
	/* Discard memblock private memory */
	memblock_discard();
#endif

	for_each_populated_zone(zone)
		set_zone_contiguous(zone);
}