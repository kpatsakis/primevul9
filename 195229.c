void intel_pmu_pebs_disable(struct perf_event *event)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct hw_perf_event *hwc = &event->hw;

	if (cpuc->n_pebs == cpuc->n_large_pebs &&
	    cpuc->n_pebs != cpuc->n_pebs_via_pt)
		intel_pmu_drain_pebs_buffer();

	cpuc->pebs_enabled &= ~(1ULL << hwc->idx);

	if ((event->hw.flags & PERF_X86_EVENT_PEBS_LDLAT) &&
	    (x86_pmu.version < 5))
		cpuc->pebs_enabled &= ~(1ULL << (hwc->idx + 32));
	else if (event->hw.flags & PERF_X86_EVENT_PEBS_ST)
		cpuc->pebs_enabled &= ~(1ULL << 63);

	intel_pmu_pebs_via_pt_disable(event);

	if (cpuc->enabled)
		wrmsrl(MSR_IA32_PEBS_ENABLE, cpuc->pebs_enabled);

	hwc->config |= ARCH_PERFMON_EVENTSEL_INT;
}