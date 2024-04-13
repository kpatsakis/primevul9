static int alloc_bts_buffer(int cpu)
{
	struct cpu_hw_events *hwev = per_cpu_ptr(&cpu_hw_events, cpu);
	struct debug_store *ds = hwev->ds;
	void *buffer, *cea;
	int max;

	if (!x86_pmu.bts)
		return 0;

	buffer = dsalloc_pages(BTS_BUFFER_SIZE, GFP_KERNEL | __GFP_NOWARN, cpu);
	if (unlikely(!buffer)) {
		WARN_ONCE(1, "%s: BTS buffer allocation failure\n", __func__);
		return -ENOMEM;
	}
	hwev->ds_bts_vaddr = buffer;
	/* Update the fixmap */
	cea = &get_cpu_entry_area(cpu)->cpu_debug_buffers.bts_buffer;
	ds->bts_buffer_base = (unsigned long) cea;
	ds_update_cea(cea, buffer, BTS_BUFFER_SIZE, PAGE_KERNEL);
	ds->bts_index = ds->bts_buffer_base;
	max = BTS_BUFFER_SIZE / BTS_RECORD_SIZE;
	ds->bts_absolute_maximum = ds->bts_buffer_base +
					max * BTS_RECORD_SIZE;
	ds->bts_interrupt_threshold = ds->bts_absolute_maximum -
					(max / 16) * BTS_RECORD_SIZE;
	return 0;
}