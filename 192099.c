static void vendor_disable_error_reporting(void)
{
	/*
	 * Don't clear on Intel or AMD CPUs. Some of these MSRs are socket-wide.
	 * Disabling them for just a single offlined CPU is bad, since it will
	 * inhibit reporting for all shared resources on the socket like the
	 * last level cache (LLC), the integrated memory controller (iMC), etc.
	 */
	if (boot_cpu_data.x86_vendor == X86_VENDOR_INTEL ||
	    boot_cpu_data.x86_vendor == X86_VENDOR_AMD)
		return;

	mce_disable_error_reporting();
}