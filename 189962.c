void kvm_mmu_free_guest_mode_roots(struct kvm *kvm, struct kvm_mmu *mmu)
{
	unsigned long roots_to_free = 0;
	hpa_t root_hpa;
	int i;

	/*
	 * This should not be called while L2 is active, L2 can't invalidate
	 * _only_ its own roots, e.g. INVVPID unconditionally exits.
	 */
	WARN_ON_ONCE(mmu->mmu_role.base.guest_mode);

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++) {
		root_hpa = mmu->prev_roots[i].hpa;
		if (!VALID_PAGE(root_hpa))
			continue;

		if (!to_shadow_page(root_hpa) ||
			to_shadow_page(root_hpa)->role.guest_mode)
			roots_to_free |= KVM_MMU_ROOT_PREVIOUS(i);
	}

	kvm_mmu_free_roots(kvm, mmu, roots_to_free);
}