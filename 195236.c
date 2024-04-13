void intel_pmu_pebs_enable(struct perf_event *event)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct hw_perf_event *hwc = &event->hw;
	struct debug_store *ds = cpuc->ds;

	hwc->config &= ~ARCH_PERFMON_EVENTSEL_INT;

	cpuc->pebs_enabled |= 1ULL << hwc->idx;

	if ((event->hw.flags & PERF_X86_EVENT_PEBS_LDLAT) && (x86_pmu.version < 5))
		cpuc->pebs_enabled |= 1ULL << (hwc->idx + 32);
	else if (event->hw.flags & PERF_X86_EVENT_PEBS_ST)
		cpuc->pebs_enabled |= 1ULL << 63;

	if (x86_pmu.intel_cap.pebs_baseline) {
		hwc->config |= ICL_EVENTSEL_ADAPTIVE;
		if (cpuc->pebs_data_cfg != cpuc->active_pebs_data_cfg) {
			wrmsrl(MSR_PEBS_DATA_CFG, cpuc->pebs_data_cfg);
			cpuc->active_pebs_data_cfg = cpuc->pebs_data_cfg;
		}
	}

	/*
	 * Use auto-reload if possible to save a MSR write in the PMI.
	 * This must be done in pmu::start(), because PERF_EVENT_IOC_PERIOD.
	 */
	if (hwc->flags & PERF_X86_EVENT_AUTO_RELOAD) {
		unsigned int idx = hwc->idx;

		if (idx >= INTEL_PMC_IDX_FIXED)
			idx = MAX_PEBS_EVENTS + (idx - INTEL_PMC_IDX_FIXED);
		ds->pebs_event_reset[idx] =
			(u64)(-hwc->sample_period) & x86_pmu.cntval_mask;
	} else {
		ds->pebs_event_reset[hwc->idx] = 0;
	}

	intel_pmu_pebs_via_pt_enable(event);
}