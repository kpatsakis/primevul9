void release_ds_buffers(void)
{
	int cpu;

	if (!x86_pmu.bts && !x86_pmu.pebs)
		return;

	for_each_possible_cpu(cpu)
		release_ds_buffer(cpu);

	for_each_possible_cpu(cpu) {
		/*
		 * Again, ignore errors from offline CPUs, they will no longer
		 * observe cpu_hw_events.ds and not program the DS_AREA when
		 * they come up.
		 */
		fini_debug_store_on_cpu(cpu);
	}

	for_each_possible_cpu(cpu) {
		release_pebs_buffer(cpu);
		release_bts_buffer(cpu);
	}
}