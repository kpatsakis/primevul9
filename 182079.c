static unsigned long kvm_get_tsc_khz(void)
{
	struct pvclock_vcpu_time_info *src;
	int cpu;
	unsigned long tsc_khz;

	cpu = get_cpu();
	src = &hv_clock[cpu].pvti;
	tsc_khz = pvclock_tsc_khz(src);
	put_cpu();
	return tsc_khz;
}