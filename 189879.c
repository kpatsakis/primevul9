int kvm_mmu_post_init_vm(struct kvm *kvm)
{
	int err;

	err = kvm_vm_create_worker_thread(kvm, kvm_nx_lpage_recovery_worker, 0,
					  "kvm-nx-lpage-recovery",
					  &kvm->arch.nx_lpage_recovery_thread);
	if (!err)
		kthread_unpark(kvm->arch.nx_lpage_recovery_thread);

	return err;
}