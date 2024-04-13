static inline u64 get_pebs_status(void *n)
{
	if (x86_pmu.intel_cap.pebs_format < 4)
		return ((struct pebs_record_nhm *)n)->status;
	return ((struct pebs_basic *)n)->applicable_counters;
}