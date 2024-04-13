void kvm_mmu_free_roots(struct kvm *kvm, struct kvm_mmu *mmu,
			ulong roots_to_free)
{
	int i;
	LIST_HEAD(invalid_list);
	bool free_active_root;

	BUILD_BUG_ON(KVM_MMU_NUM_PREV_ROOTS >= BITS_PER_LONG);

	/* Before acquiring the MMU lock, see if we need to do any real work. */
	free_active_root = (roots_to_free & KVM_MMU_ROOT_CURRENT)
		&& VALID_PAGE(mmu->root.hpa);

	if (!free_active_root) {
		for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
			if ((roots_to_free & KVM_MMU_ROOT_PREVIOUS(i)) &&
			    VALID_PAGE(mmu->prev_roots[i].hpa))
				break;

		if (i == KVM_MMU_NUM_PREV_ROOTS)
			return;
	}

	write_lock(&kvm->mmu_lock);

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
		if (roots_to_free & KVM_MMU_ROOT_PREVIOUS(i))
			mmu_free_root_page(kvm, &mmu->prev_roots[i].hpa,
					   &invalid_list);

	if (free_active_root) {
		if (to_shadow_page(mmu->root.hpa)) {
			mmu_free_root_page(kvm, &mmu->root.hpa, &invalid_list);
		} else if (mmu->pae_root) {
			for (i = 0; i < 4; ++i) {
				if (!IS_VALID_PAE_ROOT(mmu->pae_root[i]))
					continue;

				mmu_free_root_page(kvm, &mmu->pae_root[i],
						   &invalid_list);
				mmu->pae_root[i] = INVALID_PAE_ROOT;
			}
		}
		mmu->root.hpa = INVALID_PAGE;
		mmu->root.pgd = 0;
	}

	kvm_mmu_commit_zap_page(kvm, &invalid_list);
	write_unlock(&kvm->mmu_lock);
}