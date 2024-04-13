static void intel_pmu_pebs_via_pt_disable(struct perf_event *event)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);

	if (!is_pebs_pt(event))
		return;

	if (!(cpuc->pebs_enabled & ~PEBS_VIA_PT_MASK))
		cpuc->pebs_enabled &= ~PEBS_VIA_PT_MASK;
}