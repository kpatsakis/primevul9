static void intel_pmu_pebs_event_update_no_drain(struct cpu_hw_events *cpuc, int size)
{
	struct perf_event *event;
	int bit;

	/*
	 * The drain_pebs() could be called twice in a short period
	 * for auto-reload event in pmu::read(). There are no
	 * overflows have happened in between.
	 * It needs to call intel_pmu_save_and_restart_reload() to
	 * update the event->count for this case.
	 */
	for_each_set_bit(bit, (unsigned long *)&cpuc->pebs_enabled, size) {
		event = cpuc->events[bit];
		if (event->hw.flags & PERF_X86_EVENT_AUTO_RELOAD)
			intel_pmu_save_and_restart_reload(event, 0);
	}
}