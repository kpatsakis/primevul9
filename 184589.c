JVM_ActiveProcessorCount(void)
{
	PORT_ACCESS_FROM_JAVAVM(BFUjavaVM);
	jint num;

	Trc_SC_ActiveProcessorCount_Entry();
	/*
	 * This is used by Runtime.availableProcessors().
	 * Runtime.availableProcessors() by specification returns a number greater or equal to 1.
	 * RTC 112959: [was 209402] Liberty JAX-RS Default Executor poor performance.  Match reference implementation behaviour
	 * to return the bound CPUs rather than physical CPUs.
	 *
	 * This implementation should be kept consistent with jvmtiGetAvailableProcessors
	 */
	num = (jint)j9sysinfo_get_number_CPUs_by_type(J9PORT_CPU_TARGET);
	if (num < 1) {
		num = 1;
	}

	Trc_SC_ActiveProcessorCount_Exit(num);

	return num;
}