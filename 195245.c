void fini_debug_store_on_cpu(int cpu)
{
	if (!per_cpu(cpu_hw_events, cpu).ds)
		return;

	wrmsr_on_cpu(cpu, MSR_IA32_DS_AREA, 0, 0);
}