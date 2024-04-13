static inline void pebs_update_threshold(struct cpu_hw_events *cpuc)
{
	struct debug_store *ds = cpuc->ds;
	u64 threshold;
	int reserved;

	if (cpuc->n_pebs_via_pt)
		return;

	if (x86_pmu.flags & PMU_FL_PEBS_ALL)
		reserved = x86_pmu.max_pebs_events + x86_pmu.num_counters_fixed;
	else
		reserved = x86_pmu.max_pebs_events;

	if (cpuc->n_pebs == cpuc->n_large_pebs) {
		threshold = ds->pebs_absolute_maximum -
			reserved * cpuc->pebs_record_size;
	} else {
		threshold = ds->pebs_buffer_base + cpuc->pebs_record_size;
	}

	ds->pebs_interrupt_threshold = threshold;
}