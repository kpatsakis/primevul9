pebs_update_state(bool needed_cb, struct cpu_hw_events *cpuc,
		  struct perf_event *event, bool add)
{
	struct pmu *pmu = event->ctx->pmu;
	/*
	 * Make sure we get updated with the first PEBS
	 * event. It will trigger also during removal, but
	 * that does not hurt:
	 */
	bool update = cpuc->n_pebs == 1;

	if (needed_cb != pebs_needs_sched_cb(cpuc)) {
		if (!needed_cb)
			perf_sched_cb_inc(pmu);
		else
			perf_sched_cb_dec(pmu);

		update = true;
	}

	/*
	 * The PEBS record doesn't shrink on pmu::del(). Doing so would require
	 * iterating all remaining PEBS events to reconstruct the config.
	 */
	if (x86_pmu.intel_cap.pebs_baseline && add) {
		u64 pebs_data_cfg;

		/* Clear pebs_data_cfg and pebs_record_size for first PEBS. */
		if (cpuc->n_pebs == 1) {
			cpuc->pebs_data_cfg = 0;
			cpuc->pebs_record_size = sizeof(struct pebs_basic);
		}

		pebs_data_cfg = pebs_update_adaptive_cfg(event);

		/* Update pebs_record_size if new event requires more data. */
		if (pebs_data_cfg & ~cpuc->pebs_data_cfg) {
			cpuc->pebs_data_cfg |= pebs_data_cfg;
			adaptive_pebs_record_size_update();
			update = true;
		}
	}

	if (update)
		pebs_update_threshold(cpuc);
}