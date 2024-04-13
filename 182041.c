static void kvm_register_steal_time(void)
{
	int cpu = smp_processor_id();
	struct kvm_steal_time *st = &per_cpu(steal_time, cpu);

	if (!has_steal_clock)
		return;

	memset(st, 0, sizeof(*st));

	wrmsrl(MSR_KVM_STEAL_TIME, (slow_virt_to_phys(st) | KVM_MSR_ENABLED));
	pr_info("kvm-stealtime: cpu %d, msr %llx\n",
		cpu, (unsigned long long) slow_virt_to_phys(st));
}