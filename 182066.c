int kvm_register_clock(char *txt)
{
	int cpu = smp_processor_id();
	int low, high, ret;
	struct pvclock_vcpu_time_info *src;

	if (!hv_clock)
		return 0;

	src = &hv_clock[cpu].pvti;
	low = (int)slow_virt_to_phys(src) | 1;
	high = ((u64)slow_virt_to_phys(src) >> 32);
	ret = native_write_msr_safe(msr_kvm_system_time, low, high);
	printk(KERN_INFO "kvm-clock: cpu %d, msr %x:%x, %s\n",
	       cpu, high, low, txt);

	return ret;
}