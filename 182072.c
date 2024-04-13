static void kvm_get_wallclock(struct timespec *now)
{
	struct pvclock_vcpu_time_info *vcpu_time;
	int low, high;
	int cpu;

	low = (int)__pa_symbol(&wall_clock);
	high = ((u64)__pa_symbol(&wall_clock) >> 32);

	native_write_msr(msr_kvm_wall_clock, low, high);

	cpu = get_cpu();

	vcpu_time = &hv_clock[cpu].pvti;
	pvclock_read_wallclock(&wall_clock, vcpu_time, now);

	put_cpu();
}