void reserve_ds_buffers(void)
{
	int bts_err = 0, pebs_err = 0;
	int cpu;

	x86_pmu.bts_active = 0;
	x86_pmu.pebs_active = 0;

	if (!x86_pmu.bts && !x86_pmu.pebs)
		return;

	if (!x86_pmu.bts)
		bts_err = 1;

	if (!x86_pmu.pebs)
		pebs_err = 1;

	for_each_possible_cpu(cpu) {
		if (alloc_ds_buffer(cpu)) {
			bts_err = 1;
			pebs_err = 1;
		}

		if (!bts_err && alloc_bts_buffer(cpu))
			bts_err = 1;

		if (!pebs_err && alloc_pebs_buffer(cpu))
			pebs_err = 1;

		if (bts_err && pebs_err)
			break;
	}

	if (bts_err) {
		for_each_possible_cpu(cpu)
			release_bts_buffer(cpu);
	}

	if (pebs_err) {
		for_each_possible_cpu(cpu)
			release_pebs_buffer(cpu);
	}

	if (bts_err && pebs_err) {
		for_each_possible_cpu(cpu)
			release_ds_buffer(cpu);
	} else {
		if (x86_pmu.bts && !bts_err)
			x86_pmu.bts_active = 1;

		if (x86_pmu.pebs && !pebs_err)
			x86_pmu.pebs_active = 1;

		for_each_possible_cpu(cpu) {
			/*
			 * Ignores wrmsr_on_cpu() errors for offline CPUs they
			 * will get this call through intel_pmu_cpu_starting().
			 */
			init_debug_store_on_cpu(cpu);
		}
	}
}