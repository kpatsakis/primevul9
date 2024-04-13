static void __kvm_mmu_free_obsolete_roots(struct kvm *kvm, struct kvm_mmu *mmu)
{
	unsigned long roots_to_free = 0;
	int i;

	if (is_obsolete_root(kvm, mmu->root.hpa))
		roots_to_free |= KVM_MMU_ROOT_CURRENT;

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++) {
		if (is_obsolete_root(kvm, mmu->root.hpa))
			roots_to_free |= KVM_MMU_ROOT_PREVIOUS(i);
	}

	if (roots_to_free)
		kvm_mmu_free_roots(kvm, mmu, roots_to_free);
}