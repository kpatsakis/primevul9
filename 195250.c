static void release_bts_buffer(int cpu)
{
	struct cpu_hw_events *hwev = per_cpu_ptr(&cpu_hw_events, cpu);
	void *cea;

	if (!x86_pmu.bts)
		return;

	/* Clear the fixmap */
	cea = &get_cpu_entry_area(cpu)->cpu_debug_buffers.bts_buffer;
	ds_clear_cea(cea, BTS_BUFFER_SIZE);
	dsfree_pages(hwev->ds_bts_vaddr, BTS_BUFFER_SIZE);
	hwev->ds_bts_vaddr = NULL;
}