int __init kvm_setup_vsyscall_timeinfo(void)
{
#ifdef CONFIG_X86_64
	int cpu;
	int ret;
	u8 flags;
	struct pvclock_vcpu_time_info *vcpu_time;
	unsigned int size;

	if (!hv_clock)
		return 0;

	size = PAGE_ALIGN(sizeof(struct pvclock_vsyscall_time_info)*NR_CPUS);

	cpu = get_cpu();

	vcpu_time = &hv_clock[cpu].pvti;
	flags = pvclock_read_flags(vcpu_time);

	if (!(flags & PVCLOCK_TSC_STABLE_BIT)) {
		put_cpu();
		return 1;
	}

	if ((ret = pvclock_init_vsyscall(hv_clock, size))) {
		put_cpu();
		return ret;
	}

	put_cpu();

	kvm_clock.archdata.vclock_mode = VCLOCK_PVCLOCK;
#endif
	return 0;
}