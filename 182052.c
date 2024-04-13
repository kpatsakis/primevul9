void __init kvm_spinlock_init(void)
{
	if (!kvm_para_available())
		return;
	/* Does host kernel support KVM_FEATURE_PV_UNHALT? */
	if (!kvm_para_has_feature(KVM_FEATURE_PV_UNHALT))
		return;

	pv_lock_ops.lock_spinning = PV_CALLEE_SAVE(kvm_lock_spinning);
	pv_lock_ops.unlock_kick = kvm_unlock_kick;
}