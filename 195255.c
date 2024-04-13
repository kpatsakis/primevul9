void intel_pmu_enable_bts(u64 config)
{
	unsigned long debugctlmsr;

	debugctlmsr = get_debugctlmsr();

	debugctlmsr |= DEBUGCTLMSR_TR;
	debugctlmsr |= DEBUGCTLMSR_BTS;
	if (config & ARCH_PERFMON_EVENTSEL_INT)
		debugctlmsr |= DEBUGCTLMSR_BTINT;

	if (!(config & ARCH_PERFMON_EVENTSEL_OS))
		debugctlmsr |= DEBUGCTLMSR_BTS_OFF_OS;

	if (!(config & ARCH_PERFMON_EVENTSEL_USR))
		debugctlmsr |= DEBUGCTLMSR_BTS_OFF_USR;

	update_debugctlmsr(debugctlmsr);
}