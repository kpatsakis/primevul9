static cycle_t kvm_clock_read(void)
{
	struct pvclock_vcpu_time_info *src;
	cycle_t ret;
	int cpu;

	preempt_disable_notrace();
	cpu = smp_processor_id();
	src = &hv_clock[cpu].pvti;
	ret = pvclock_clocksource_read(src);
	preempt_enable_notrace();
	return ret;
}