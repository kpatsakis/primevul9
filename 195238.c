static void intel_pmu_pebs_via_pt_enable(struct perf_event *event)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct hw_perf_event *hwc = &event->hw;
	struct debug_store *ds = cpuc->ds;

	if (!is_pebs_pt(event))
		return;

	if (!(event->hw.flags & PERF_X86_EVENT_LARGE_PEBS))
		cpuc->pebs_enabled |= PEBS_PMI_AFTER_EACH_RECORD;

	cpuc->pebs_enabled |= PEBS_OUTPUT_PT;

	wrmsrl(MSR_RELOAD_PMC0 + hwc->idx, ds->pebs_event_reset[hwc->idx]);
}