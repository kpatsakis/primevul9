static inline void intel_pmu_drain_pebs_buffer(void)
{
	struct perf_sample_data data;

	x86_pmu.drain_pebs(NULL, &data);
}