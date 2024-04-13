static __init int kvm_spinlock_init_jump(void)
{
	if (!kvm_para_available())
		return 0;
	if (!kvm_para_has_feature(KVM_FEATURE_PV_UNHALT))
		return 0;

	static_key_slow_inc(&paravirt_ticketlocks_enabled);
	printk(KERN_INFO "KVM setup paravirtual spinlock\n");

	return 0;
}