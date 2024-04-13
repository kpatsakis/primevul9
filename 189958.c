static bool fast_pgd_switch(struct kvm *kvm, struct kvm_mmu *mmu,
			    gpa_t new_pgd, union kvm_mmu_page_role new_role)
{
	/*
	 * For now, limit the caching to 64-bit hosts+VMs in order to avoid
	 * having to deal with PDPTEs. We may add support for 32-bit hosts/VMs
	 * later if necessary.
	 */
	if (VALID_PAGE(mmu->root.hpa) && !to_shadow_page(mmu->root.hpa))
		kvm_mmu_free_roots(kvm, mmu, KVM_MMU_ROOT_CURRENT);

	if (VALID_PAGE(mmu->root.hpa))
		return cached_root_find_and_keep_current(kvm, mmu, new_pgd, new_role);
	else
		return cached_root_find_without_current(kvm, mmu, new_pgd, new_role);
}