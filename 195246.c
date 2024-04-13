static void intel_pmu_drain_pebs_core(struct pt_regs *iregs, struct perf_sample_data *data)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct debug_store *ds = cpuc->ds;
	struct perf_event *event = cpuc->events[0]; /* PMC0 only */
	struct pebs_record_core *at, *top;
	int n;

	if (!x86_pmu.pebs_active)
		return;

	at  = (struct pebs_record_core *)(unsigned long)ds->pebs_buffer_base;
	top = (struct pebs_record_core *)(unsigned long)ds->pebs_index;

	/*
	 * Whatever else happens, drain the thing
	 */
	ds->pebs_index = ds->pebs_buffer_base;

	if (!test_bit(0, cpuc->active_mask))
		return;

	WARN_ON_ONCE(!event);

	if (!event->attr.precise_ip)
		return;

	n = top - at;
	if (n <= 0) {
		if (event->hw.flags & PERF_X86_EVENT_AUTO_RELOAD)
			intel_pmu_save_and_restart_reload(event, 0);
		return;
	}

	__intel_pmu_pebs_event(event, iregs, data, at, top, 0, n,
			       setup_pebs_fixed_sample_data);
}