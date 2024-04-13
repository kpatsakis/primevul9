static void release_pebs_buffer(int cpu)
{
	struct cpu_hw_events *hwev = per_cpu_ptr(&cpu_hw_events, cpu);
	void *cea;

	if (!x86_pmu.pebs)
		return;

	kfree(per_cpu(insn_buffer, cpu));
	per_cpu(insn_buffer, cpu) = NULL;

	/* Clear the fixmap */
	cea = &get_cpu_entry_area(cpu)->cpu_debug_buffers.pebs_buffer;
	ds_clear_cea(cea, x86_pmu.pebs_buffer_size);
	dsfree_pages(hwev->ds_pebs_vaddr, x86_pmu.pebs_buffer_size);
	hwev->ds_pebs_vaddr = NULL;
}