void __init intel_ds_init(void)
{
	/*
	 * No support for 32bit formats
	 */
	if (!boot_cpu_has(X86_FEATURE_DTES64))
		return;

	x86_pmu.bts  = boot_cpu_has(X86_FEATURE_BTS);
	x86_pmu.pebs = boot_cpu_has(X86_FEATURE_PEBS);
	x86_pmu.pebs_buffer_size = PEBS_BUFFER_SIZE;
	if (x86_pmu.version <= 4)
		x86_pmu.pebs_no_isolation = 1;

	if (x86_pmu.pebs) {
		char pebs_type = x86_pmu.intel_cap.pebs_trap ?  '+' : '-';
		char *pebs_qual = "";
		int format = x86_pmu.intel_cap.pebs_format;

		if (format < 4)
			x86_pmu.intel_cap.pebs_baseline = 0;

		switch (format) {
		case 0:
			pr_cont("PEBS fmt0%c, ", pebs_type);
			x86_pmu.pebs_record_size = sizeof(struct pebs_record_core);
			/*
			 * Using >PAGE_SIZE buffers makes the WRMSR to
			 * PERF_GLOBAL_CTRL in intel_pmu_enable_all()
			 * mysteriously hang on Core2.
			 *
			 * As a workaround, we don't do this.
			 */
			x86_pmu.pebs_buffer_size = PAGE_SIZE;
			x86_pmu.drain_pebs = intel_pmu_drain_pebs_core;
			break;

		case 1:
			pr_cont("PEBS fmt1%c, ", pebs_type);
			x86_pmu.pebs_record_size = sizeof(struct pebs_record_nhm);
			x86_pmu.drain_pebs = intel_pmu_drain_pebs_nhm;
			break;

		case 2:
			pr_cont("PEBS fmt2%c, ", pebs_type);
			x86_pmu.pebs_record_size = sizeof(struct pebs_record_hsw);
			x86_pmu.drain_pebs = intel_pmu_drain_pebs_nhm;
			break;

		case 3:
			pr_cont("PEBS fmt3%c, ", pebs_type);
			x86_pmu.pebs_record_size =
						sizeof(struct pebs_record_skl);
			x86_pmu.drain_pebs = intel_pmu_drain_pebs_nhm;
			x86_pmu.large_pebs_flags |= PERF_SAMPLE_TIME;
			break;

		case 4:
			x86_pmu.drain_pebs = intel_pmu_drain_pebs_icl;
			x86_pmu.pebs_record_size = sizeof(struct pebs_basic);
			if (x86_pmu.intel_cap.pebs_baseline) {
				x86_pmu.large_pebs_flags |=
					PERF_SAMPLE_BRANCH_STACK |
					PERF_SAMPLE_TIME;
				x86_pmu.flags |= PMU_FL_PEBS_ALL;
				pebs_qual = "-baseline";
				x86_get_pmu()->capabilities |= PERF_PMU_CAP_EXTENDED_REGS;
			} else {
				/* Only basic record supported */
				x86_pmu.large_pebs_flags &=
					~(PERF_SAMPLE_ADDR |
					  PERF_SAMPLE_TIME |
					  PERF_SAMPLE_DATA_SRC |
					  PERF_SAMPLE_TRANSACTION |
					  PERF_SAMPLE_REGS_USER |
					  PERF_SAMPLE_REGS_INTR);
			}
			pr_cont("PEBS fmt4%c%s, ", pebs_type, pebs_qual);

			if (x86_pmu.intel_cap.pebs_output_pt_available) {
				pr_cont("PEBS-via-PT, ");
				x86_get_pmu()->capabilities |= PERF_PMU_CAP_AUX_OUTPUT;
			}

			break;

		default:
			pr_cont("no PEBS fmt%d%c, ", format, pebs_type);
			x86_pmu.pebs = 0;
		}
	}
}