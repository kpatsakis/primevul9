static bool cached_root_find_and_keep_current(struct kvm *kvm, struct kvm_mmu *mmu,
					      gpa_t new_pgd,
					      union kvm_mmu_page_role new_role)
{
	uint i;

	if (is_root_usable(&mmu->root, new_pgd, new_role))
		return true;

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++) {
		/*
		 * The swaps end up rotating the cache like this:
		 *   C   0 1 2 3   (on entry to the function)
		 *   0   C 1 2 3
		 *   1   C 0 2 3
		 *   2   C 0 1 3
		 *   3   C 0 1 2   (on exit from the loop)
		 */
		swap(mmu->root, mmu->prev_roots[i]);
		if (is_root_usable(&mmu->root, new_pgd, new_role))
			return true;
	}

	kvm_mmu_free_roots(kvm, mmu, KVM_MMU_ROOT_CURRENT);
	return false;
}