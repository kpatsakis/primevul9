static u64 pebs_update_adaptive_cfg(struct perf_event *event)
{
	struct perf_event_attr *attr = &event->attr;
	u64 sample_type = attr->sample_type;
	u64 pebs_data_cfg = 0;
	bool gprs, tsx_weight;

	if (!(sample_type & ~(PERF_SAMPLE_IP|PERF_SAMPLE_TIME)) &&
	    attr->precise_ip > 1)
		return pebs_data_cfg;

	if (sample_type & PERF_PEBS_MEMINFO_TYPE)
		pebs_data_cfg |= PEBS_DATACFG_MEMINFO;

	/*
	 * We need GPRs when:
	 * + user requested them
	 * + precise_ip < 2 for the non event IP
	 * + For RTM TSX weight we need GPRs for the abort code.
	 */
	gprs = (sample_type & PERF_SAMPLE_REGS_INTR) &&
	       (attr->sample_regs_intr & PEBS_GP_REGS);

	tsx_weight = (sample_type & PERF_SAMPLE_WEIGHT_TYPE) &&
		     ((attr->config & INTEL_ARCH_EVENT_MASK) ==
		      x86_pmu.rtm_abort_event);

	if (gprs || (attr->precise_ip < 2) || tsx_weight)
		pebs_data_cfg |= PEBS_DATACFG_GP;

	if ((sample_type & PERF_SAMPLE_REGS_INTR) &&
	    (attr->sample_regs_intr & PERF_REG_EXTENDED_MASK))
		pebs_data_cfg |= PEBS_DATACFG_XMMS;

	if (sample_type & PERF_SAMPLE_BRANCH_STACK) {
		/*
		 * For now always log all LBRs. Could configure this
		 * later.
		 */
		pebs_data_cfg |= PEBS_DATACFG_LBRS |
			((x86_pmu.lbr_nr-1) << PEBS_DATACFG_LBR_SHIFT);
	}

	return pebs_data_cfg;
}