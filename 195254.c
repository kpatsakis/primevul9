void intel_pmu_auto_reload_read(struct perf_event *event)
{
	WARN_ON(!(event->hw.flags & PERF_X86_EVENT_AUTO_RELOAD));

	perf_pmu_disable(event->pmu);
	intel_pmu_drain_pebs_buffer();
	perf_pmu_enable(event->pmu);
}