static bool cached_root_find_without_current(struct kvm *kvm, struct kvm_mmu *mmu,
					     gpa_t new_pgd,
					     union kvm_mmu_page_role new_role)
{
	uint i;

	for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
		if (is_root_usable(&mmu->prev_roots[i], new_pgd, new_role))
			goto hit;

	return false;

hit:
	swap(mmu->root, mmu->prev_roots[i]);
	/* Bubble up the remaining roots.  */
	for (; i < KVM_MMU_NUM_PREV_ROOTS - 1; i++)
		mmu->prev_roots[i] = mmu->prev_roots[i + 1];
	mmu->prev_roots[i].hpa = INVALID_PAGE;
	return true;
}