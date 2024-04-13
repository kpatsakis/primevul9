void intel_pmu_pebs_add(struct perf_event *event)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct hw_perf_event *hwc = &event->hw;
	bool needed_cb = pebs_needs_sched_cb(cpuc);

	cpuc->n_pebs++;
	if (hwc->flags & PERF_X86_EVENT_LARGE_PEBS)
		cpuc->n_large_pebs++;
	if (hwc->flags & PERF_X86_EVENT_PEBS_VIA_PT)
		cpuc->n_pebs_via_pt++;

	pebs_update_state(needed_cb, cpuc, event, true);
}