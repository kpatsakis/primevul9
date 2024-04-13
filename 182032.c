void kvm_disable_steal_time(void)
{
	if (!has_steal_clock)
		return;

	wrmsr(MSR_KVM_STEAL_TIME, 0, 0);
}