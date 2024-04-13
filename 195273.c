static inline bool pebs_needs_sched_cb(struct cpu_hw_events *cpuc)
{
	if (cpuc->n_pebs == cpuc->n_pebs_via_pt)
		return false;

	return cpuc->n_pebs && (cpuc->n_pebs == cpuc->n_large_pebs);
}