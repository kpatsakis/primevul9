void intel_pmu_pebs_sched_task(struct perf_event_context *ctx, bool sched_in)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);

	if (!sched_in && pebs_needs_sched_cb(cpuc))
		intel_pmu_drain_pebs_buffer();
}