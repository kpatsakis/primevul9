static void intel_pmu_drain_pebs_icl(struct pt_regs *iregs, struct perf_sample_data *data)
{
	short counts[INTEL_PMC_IDX_FIXED + MAX_FIXED_PEBS_EVENTS] = {};
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct debug_store *ds = cpuc->ds;
	struct perf_event *event;
	void *base, *at, *top;
	int bit, size;
	u64 mask;

	if (!x86_pmu.pebs_active)
		return;

	base = (struct pebs_basic *)(unsigned long)ds->pebs_buffer_base;
	top = (struct pebs_basic *)(unsigned long)ds->pebs_index;

	ds->pebs_index = ds->pebs_buffer_base;

	mask = ((1ULL << x86_pmu.max_pebs_events) - 1) |
	       (((1ULL << x86_pmu.num_counters_fixed) - 1) << INTEL_PMC_IDX_FIXED);
	size = INTEL_PMC_IDX_FIXED + x86_pmu.num_counters_fixed;

	if (unlikely(base >= top)) {
		intel_pmu_pebs_event_update_no_drain(cpuc, size);
		return;
	}

	for (at = base; at < top; at += cpuc->pebs_record_size) {
		u64 pebs_status;

		pebs_status = get_pebs_status(at) & cpuc->pebs_enabled;
		pebs_status &= mask;

		for_each_set_bit(bit, (unsigned long *)&pebs_status, size)
			counts[bit]++;
	}

	for_each_set_bit(bit, (unsigned long *)&mask, size) {
		if (counts[bit] == 0)
			continue;

		event = cpuc->events[bit];
		if (WARN_ON_ONCE(!event))
			continue;

		if (WARN_ON_ONCE(!event->attr.precise_ip))
			continue;

		__intel_pmu_pebs_event(event, iregs, data, base,
				       top, bit, counts[bit],
				       setup_pebs_adaptive_sample_data);
	}
}