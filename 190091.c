void kvm_mmu_sync_prev_roots(struct kvm_vcpu *vcpu)
{
	unsigned long roots_to_free = 0;
	int i;

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
		if (is_unsync_root(vcpu->arch.mmu->prev_roots[i].hpa))
			roots_to_free |= KVM_MMU_ROOT_PREVIOUS(i);

	/* sync prev_roots by simply freeing them */
	kvm_mmu_free_roots(vcpu->kvm, vcpu->arch.mmu, roots_to_free);
}