get_next_pebs_record_by_bit(void *base, void *top, int bit)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	void *at;
	u64 pebs_status;

	/*
	 * fmt0 does not have a status bitfield (does not use
	 * perf_record_nhm format)
	 */
	if (x86_pmu.intel_cap.pebs_format < 1)
		return base;

	if (base == NULL)
		return NULL;

	for (at = base; at < top; at += cpuc->pebs_record_size) {
		unsigned long status = get_pebs_status(at);

		if (test_bit(bit, (unsigned long *)&status)) {
			/* PEBS v3 has accurate status bits */
			if (x86_pmu.intel_cap.pebs_format >= 3)
				return at;

			if (status == (1 << bit))
				return at;

			/* clear non-PEBS bit and re-check */
			pebs_status = status & cpuc->pebs_enabled;
			pebs_status &= PEBS_COUNTER_MASK;
			if (pebs_status == (1 << bit))
				return at;
		}
	}
	return NULL;
}