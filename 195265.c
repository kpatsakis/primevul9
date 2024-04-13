static int alloc_pebs_buffer(int cpu)
{
	struct cpu_hw_events *hwev = per_cpu_ptr(&cpu_hw_events, cpu);
	struct debug_store *ds = hwev->ds;
	size_t bsiz = x86_pmu.pebs_buffer_size;
	int max, node = cpu_to_node(cpu);
	void *buffer, *insn_buff, *cea;

	if (!x86_pmu.pebs)
		return 0;

	buffer = dsalloc_pages(bsiz, GFP_KERNEL, cpu);
	if (unlikely(!buffer))
		return -ENOMEM;

	/*
	 * HSW+ already provides us the eventing ip; no need to allocate this
	 * buffer then.
	 */
	if (x86_pmu.intel_cap.pebs_format < 2) {
		insn_buff = kzalloc_node(PEBS_FIXUP_SIZE, GFP_KERNEL, node);
		if (!insn_buff) {
			dsfree_pages(buffer, bsiz);
			return -ENOMEM;
		}
		per_cpu(insn_buffer, cpu) = insn_buff;
	}
	hwev->ds_pebs_vaddr = buffer;
	/* Update the cpu entry area mapping */
	cea = &get_cpu_entry_area(cpu)->cpu_debug_buffers.pebs_buffer;
	ds->pebs_buffer_base = (unsigned long) cea;
	ds_update_cea(cea, buffer, bsiz, PAGE_KERNEL);
	ds->pebs_index = ds->pebs_buffer_base;
	max = x86_pmu.pebs_record_size * (bsiz / x86_pmu.pebs_record_size);
	ds->pebs_absolute_maximum = ds->pebs_buffer_base + max;
	return 0;
}