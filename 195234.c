void intel_pmu_pebs_enable_all(void)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);

	if (cpuc->pebs_enabled)
		wrmsrl(MSR_IA32_PEBS_ENABLE, cpuc->pebs_enabled);
}