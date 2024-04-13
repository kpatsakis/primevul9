static int alloc_ds_buffer(int cpu)
{
	struct debug_store *ds = &get_cpu_entry_area(cpu)->cpu_debug_store;

	memset(ds, 0, sizeof(*ds));
	per_cpu(cpu_hw_events, cpu).ds = ds;
	return 0;
}