static void setup_pebs_fixed_sample_data(struct perf_event *event,
				   struct pt_regs *iregs, void *__pebs,
				   struct perf_sample_data *data,
				   struct pt_regs *regs)
{
	/*
	 * We cast to the biggest pebs_record but are careful not to
	 * unconditionally access the 'extra' entries.
	 */
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct pebs_record_skl *pebs = __pebs;
	u64 sample_type;
	int fll;

	if (pebs == NULL)
		return;

	sample_type = event->attr.sample_type;
	fll = event->hw.flags & PERF_X86_EVENT_PEBS_LDLAT;

	perf_sample_data_init(data, 0, event->hw.last_period);

	data->period = event->hw.last_period;

	/*
	 * Use latency for weight (only avail with PEBS-LL)
	 */
	if (fll && (sample_type & PERF_SAMPLE_WEIGHT_TYPE))
		data->weight.full = pebs->lat;

	/*
	 * data.data_src encodes the data source
	 */
	if (sample_type & PERF_SAMPLE_DATA_SRC)
		data->data_src.val = get_data_src(event, pebs->dse);

	/*
	 * We must however always use iregs for the unwinder to stay sane; the
	 * record BP,SP,IP can point into thin air when the record is from a
	 * previous PMI context or an (I)RET happened between the record and
	 * PMI.
	 */
	if (sample_type & PERF_SAMPLE_CALLCHAIN)
		data->callchain = perf_callchain(event, iregs);

	/*
	 * We use the interrupt regs as a base because the PEBS record does not
	 * contain a full regs set, specifically it seems to lack segment
	 * descriptors, which get used by things like user_mode().
	 *
	 * In the simple case fix up only the IP for PERF_SAMPLE_IP.
	 */
	*regs = *iregs;

	/*
	 * Initialize regs_>flags from PEBS,
	 * Clear exact bit (which uses x86 EFLAGS Reserved bit 3),
	 * i.e., do not rely on it being zero:
	 */
	regs->flags = pebs->flags & ~PERF_EFLAGS_EXACT;

	if (sample_type & PERF_SAMPLE_REGS_INTR) {
		regs->ax = pebs->ax;
		regs->bx = pebs->bx;
		regs->cx = pebs->cx;
		regs->dx = pebs->dx;
		regs->si = pebs->si;
		regs->di = pebs->di;

		regs->bp = pebs->bp;
		regs->sp = pebs->sp;

#ifndef CONFIG_X86_32
		regs->r8 = pebs->r8;
		regs->r9 = pebs->r9;
		regs->r10 = pebs->r10;
		regs->r11 = pebs->r11;
		regs->r12 = pebs->r12;
		regs->r13 = pebs->r13;
		regs->r14 = pebs->r14;
		regs->r15 = pebs->r15;
#endif
	}

	if (event->attr.precise_ip > 1) {
		/*
		 * Haswell and later processors have an 'eventing IP'
		 * (real IP) which fixes the off-by-1 skid in hardware.
		 * Use it when precise_ip >= 2 :
		 */
		if (x86_pmu.intel_cap.pebs_format >= 2) {
			set_linear_ip(regs, pebs->real_ip);
			regs->flags |= PERF_EFLAGS_EXACT;
		} else {
			/* Otherwise, use PEBS off-by-1 IP: */
			set_linear_ip(regs, pebs->ip);

			/*
			 * With precise_ip >= 2, try to fix up the off-by-1 IP
			 * using the LBR. If successful, the fixup function
			 * corrects regs->ip and calls set_linear_ip() on regs:
			 */
			if (intel_pmu_pebs_fixup_ip(regs))
				regs->flags |= PERF_EFLAGS_EXACT;
		}
	} else {
		/*
		 * When precise_ip == 1, return the PEBS off-by-1 IP,
		 * no fixup attempted:
		 */
		set_linear_ip(regs, pebs->ip);
	}


	if ((sample_type & PERF_SAMPLE_ADDR_TYPE) &&
	    x86_pmu.intel_cap.pebs_format >= 1)
		data->addr = pebs->dla;

	if (x86_pmu.intel_cap.pebs_format >= 2) {
		/* Only set the TSX weight when no memory weight. */
		if ((sample_type & PERF_SAMPLE_WEIGHT_TYPE) && !fll)
			data->weight.full = intel_get_tsx_weight(pebs->tsx_tuning);

		if (sample_type & PERF_SAMPLE_TRANSACTION)
			data->txn = intel_get_tsx_transaction(pebs->tsx_tuning,
							      pebs->ax);
	}

	/*
	 * v3 supplies an accurate time stamp, so we use that
	 * for the time stamp.
	 *
	 * We can only do this for the default trace clock.
	 */
	if (x86_pmu.intel_cap.pebs_format >= 3 &&
		event->attr.use_clockid == 0)
		data->time = native_sched_clock_from_tsc(pebs->tsc);

	if (has_branch_stack(event))
		data->br_stack = &cpuc->lbr_stack;
}