void intel_pmu_disable_bts(void)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	unsigned long debugctlmsr;

	if (!cpuc->ds)
		return;

	debugctlmsr = get_debugctlmsr();

	debugctlmsr &=
		~(DEBUGCTLMSR_TR | DEBUGCTLMSR_BTS | DEBUGCTLMSR_BTINT |
		  DEBUGCTLMSR_BTS_OFF_OS | DEBUGCTLMSR_BTS_OFF_USR);

	update_debugctlmsr(debugctlmsr);
}