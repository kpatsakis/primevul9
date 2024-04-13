void kvm_mmu_pre_destroy_vm(struct kvm *kvm)
{
	if (kvm->arch.nx_lpage_recovery_thread)
		kthread_stop(kvm->arch.nx_lpage_recovery_thread);
}