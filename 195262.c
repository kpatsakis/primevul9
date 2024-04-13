static void setup_pebs_adaptive_sample_data(struct perf_event *event,
					    struct pt_regs *iregs, void *__pebs,
					    struct perf_sample_data *data,
					    struct pt_regs *regs)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	struct pebs_basic *basic = __pebs;
	void *next_record = basic + 1;
	u64 sample_type;
	u64 format_size;
	struct pebs_meminfo *meminfo = NULL;
	struct pebs_gprs *gprs = NULL;
	struct x86_perf_regs *perf_regs;

	if (basic == NULL)
		return;

	perf_regs = container_of(regs, struct x86_perf_regs, regs);
	perf_regs->xmm_regs = NULL;

	sample_type = event->attr.sample_type;
	format_size = basic->format_size;
	perf_sample_data_init(data, 0, event->hw.last_period);
	data->period = event->hw.last_period;

	if (event->attr.use_clockid == 0)
		data->time = native_sched_clock_from_tsc(basic->tsc);

	/*
	 * We must however always use iregs for the unwinder to stay sane; the
	 * record BP,SP,IP can point into thin air when the record is from a
	 * previous PMI context or an (I)RET happened between the record and
	 * PMI.
	 */
	if (sample_type & PERF_SAMPLE_CALLCHAIN)
		data->callchain = perf_callchain(event, iregs);

	*regs = *iregs;
	/* The ip in basic is EventingIP */
	set_linear_ip(regs, basic->ip);
	regs->flags = PERF_EFLAGS_EXACT;

	/*
	 * The record for MEMINFO is in front of GP
	 * But PERF_SAMPLE_TRANSACTION needs gprs->ax.
	 * Save the pointer here but process later.
	 */
	if (format_size & PEBS_DATACFG_MEMINFO) {
		meminfo = next_record;
		next_record = meminfo + 1;
	}

	if (format_size & PEBS_DATACFG_GP) {
		gprs = next_record;
		next_record = gprs + 1;

		if (event->attr.precise_ip < 2) {
			set_linear_ip(regs, gprs->ip);
			regs->flags &= ~PERF_EFLAGS_EXACT;
		}

		if (sample_type & PERF_SAMPLE_REGS_INTR)
			adaptive_pebs_save_regs(regs, gprs);
	}

	if (format_size & PEBS_DATACFG_MEMINFO) {
		if (sample_type & PERF_SAMPLE_WEIGHT_TYPE) {
			u64 weight = meminfo->latency;

			if (x86_pmu.flags & PMU_FL_INSTR_LATENCY) {
				data->weight.var2_w = weight & PEBS_LATENCY_MASK;
				weight >>= PEBS_CACHE_LATENCY_OFFSET;
			}

			/*
			 * Although meminfo::latency is defined as a u64,
			 * only the lower 32 bits include the valid data
			 * in practice on Ice Lake and earlier platforms.
			 */
			if (sample_type & PERF_SAMPLE_WEIGHT) {
				data->weight.full = weight ?:
					intel_get_tsx_weight(meminfo->tsx_tuning);
			} else {
				data->weight.var1_dw = (u32)(weight & PEBS_LATENCY_MASK) ?:
					intel_get_tsx_weight(meminfo->tsx_tuning);
			}
		}

		if (sample_type & PERF_SAMPLE_DATA_SRC)
			data->data_src.val = get_data_src(event, meminfo->aux);

		if (sample_type & PERF_SAMPLE_ADDR_TYPE)
			data->addr = meminfo->address;

		if (sample_type & PERF_SAMPLE_TRANSACTION)
			data->txn = intel_get_tsx_transaction(meminfo->tsx_tuning,
							  gprs ? gprs->ax : 0);
	}

	if (format_size & PEBS_DATACFG_XMMS) {
		struct pebs_xmm *xmm = next_record;

		next_record = xmm + 1;
		perf_regs->xmm_regs = xmm->xmm;
	}

	if (format_size & PEBS_DATACFG_LBRS) {
		struct lbr_entry *lbr = next_record;
		int num_lbr = ((format_size >> PEBS_DATACFG_LBR_SHIFT)
					& 0xff) + 1;
		next_record = next_record + num_lbr * sizeof(struct lbr_entry);

		if (has_branch_stack(event)) {
			intel_pmu_store_pebs_lbrs(lbr);
			data->br_stack = &cpuc->lbr_stack;
		}
	}

	WARN_ONCE(next_record != __pebs + (format_size >> 48),
			"PEBS record size %llu, expected %llu, config %llx\n",
			format_size >> 48,
			(u64)(next_record - __pebs),
			basic->format_size);
}